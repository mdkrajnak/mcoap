/** 
 * @file
 * @ingroup socket
 * @{
 */

/* Socket  module for Unix */
#ifndef _WIN32
#include <string.h> 
#include <signal.h>

#include "mnet/mn_socket.h"

/*
 * Wait for readable/writable/connected socket with timeout
 */
#ifdef SOCKET_POLL
#include <sys/poll.h>

#define WAITFD_R        POLLIN
#define WAITFD_W        POLLOUT
#define WAITFD_C        (POLLIN|POLLOUT)
int mn_socket_waitfd(mn_socket_t* sock, int sw, mn_timeout_t* tout) {
    int ret;
    struct pollfd pfd;
    pfd.fd = *sock;
    pfd.events = sw;
    pfd.revents = 0;
    
    /* optimize timeout == 0 case */
    if (mn_timeout_iszero(tout)) return MN_TIMEOUT;  
    
    do {
        int t = (int)(mn_timeout_getretry(tout)*1e3);
        ret = poll(&pfd, 1, t >= 0? t: -1);
    } while (ret == -1 && errno == EINTR);
    if (ret == -1) return errno;
    if (ret == 0) return MN_TIMEOUT;
    if (sw == WAITFD_C && (pfd.revents & (POLLIN|POLLERR))) return MN_CLOSED;
    return MN_DONE;
}
#else

#define WAITFD_R        1
#define WAITFD_W        2
#define WAITFD_C        (WAITFD_R|WAITFD_W)

int mn_socket_waitfd(mn_socket_t* sock, int sw, mn_timeout_t* tout) {
    int ret;
    fd_set rfds, wfds, *rp, *wp;
    struct timeval tv, *tp;
    double t;
    if (mn_timeout_iszero(tout)) return MN_TIMEOUT;  /* optimize timeout == 0 case */
    do {
        /* must set bits within loop, because select may have modifed them */
        rp = wp = NULL;
        if (sw & WAITFD_R) { FD_ZERO(&rfds); FD_SET(*sock, &rfds); rp = &rfds; }
        if (sw & WAITFD_W) { FD_ZERO(&wfds); FD_SET(*sock, &wfds); wp = &wfds; }
        t = mn_timeout_getretry(tout);
        tp = NULL;
        if (t >= 0.0) {
            tv.tv_sec = (int)t;
            tv.tv_usec = (int)((t-tv.tv_sec)*1.0e6);
            tp = &tv;
        }
        ret = select(*sock+1, rp, wp, NULL, tp);
    } while (ret == -1 && errno == EINTR);
    if (ret == -1) return errno;
    if (ret == 0) return MN_TIMEOUT;
    if (sw == WAITFD_C && FD_ISSET(*sock, &rfds)) return MN_CLOSED;
    return MN_DONE;
}
#endif


/**
 * Initializes module 
 */
int mn_socket_open() {
    /* instals a handler to ignore sigpipe or it will crash us */
    signal(SIGPIPE, SIG_IGN);
    return MN_DONE;
}

/**
 * Close module 
 */
int mn_socket_close() {
    return 1;
}

/**
 * Close and invalidate socket
 */
void mn_socket_destroy(mn_socket_t* sock) {
    if (*sock != MN_SOCKET_INVALID) {
        mn_socket_setblocking(sock);
        close(*sock);
        *sock = MN_SOCKET_INVALID;
    }
}

/**
 * Creates and sets up a socket
 */
int mn_socket_create(mn_socket_t* sock, int domain, int type, int protocol) {
    int reuse = 1;
    *sock = socket(domain, type, protocol);
    if (*sock == MN_SOCKET_INVALID) return errno;
    
    /* Set reuseaddr so we can restart the socket in case of a crash. */
    setsockopt(*sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    return MN_DONE; 
}

/**
 * Binds or returns error message
 */
int mn_socket_bind(mn_socket_t* sock, sockaddr_t *addr, socklen_t len) {
    int err = MN_DONE;
    mn_socket_setblocking(sock);
    if (bind(*sock, addr, len) < 0) err = errno; 
    mn_socket_setnonblocking(sock);
    return err;
}

/**
 * Initialize socket for listening.
 */
int mn_socket_listen(mn_socket_t* sock, int backlog) {
    int err = MN_DONE; 
    mn_socket_setblocking(sock);
    if (listen(*sock, backlog)) err = errno; 
    mn_socket_setnonblocking(sock);
    return err;
}

/**
 * Shutdown socket. 
 */
void mn_socket_shutdown(mn_socket_t* sock, int how) {
    mn_socket_setblocking(sock);
    shutdown(*sock, how);
    mn_socket_setnonblocking(sock);
}

/**
 * Connects or returns error message
 */
int mn_socket_connect(mn_socket_t* sock, sockaddr_t* addr, socklen_t len, mn_timeout_t* tout) {
    int err;
    
    /* avoid calling on closed sockets */
    if (*sock == MN_SOCKET_INVALID) return MN_CLOSED;
    
    /* call connect until done or failed without being interrupted */
    do if (connect(*sock, addr, len) == 0) return MN_DONE;
    
    while ((err = errno) == EINTR);
    
    /* if connection failed immediately, return error code */
    if (err != EINPROGRESS && err != EAGAIN) return err; 
    
    /* zero timeout case optimization */
    if (mn_timeout_iszero(tout)) return MN_TIMEOUT;
    
    /* wait until we have the result of the connection attempt or timeout */
    err = mn_socket_waitfd(sock, WAITFD_C, tout);
    if (err == MN_CLOSED) {
        if (recv(*sock, (char *) &err, 0, 0) == 0) return MN_DONE;
        else return errno;
    } else return err;
}

/**
 * Accept with timeout
 */
int mn_socket_accept(mn_socket_t* sock, mn_socket_t* asock, sockaddr_t* addr, socklen_t* len, mn_timeout_t* tout) {
    sockaddr_t daddr;
    socklen_t dlen = sizeof(daddr);
    if (*sock == MN_SOCKET_INVALID) return MN_CLOSED; 
    if (!addr) addr = &daddr;
    if (!len) len = &dlen;
    for ( ;; ) {
        int err;
        if ((*asock = accept(*sock, addr, len)) != MN_SOCKET_INVALID) return MN_DONE;
        err = errno;
        if (err == EINTR) continue;
        if (err != EAGAIN && err != ECONNABORTED) return err;
        if ((err = mn_socket_waitfd(sock, WAITFD_R, tout)) != MN_DONE) return err;
    }
    /* can't reach here */
    return MN_UNKNOWN;
}

/**
 * Send with timeout
 */
int mn_socket_send(mn_socket_t* sock, const char *data, size_t count, size_t *sent, mn_timeout_t* tout) {
    int err;
    *sent = 0;
    
    /* avoid making system calls on closed sockets */
    if (*sock == MN_SOCKET_INVALID) return MN_CLOSED;
    
    /* loop until we send something or we give up on error */
    for ( ;; ) {
        long put = (long) send(*sock, data, count, 0);
        
        /* if we sent anything, we are done */
        if (put > 0) {
            *sent = put;
            return MN_DONE;
        }
        err = errno;
        
        /* send can't really return 0, but EPIPE means the connection was closed */
        if (put == 0 || err == EPIPE) return MN_CLOSED;
        
        /* we call was interrupted, just try again */
        if (err == EINTR) continue;
        
        /* if failed fatal reason, report error */
        if (err != EAGAIN) return err;
        
        /* wait until we can send something or we timeout */
        if ((err = mn_socket_waitfd(sock, WAITFD_W, tout)) != MN_DONE) return err;
    }
    /* can't reach here */
    return MN_UNKNOWN;
}

/**
 * Sendto with timeout
 */
int mn_socket_sendto(mn_socket_t* sock, const char *data, size_t count, size_t *sent, 
        sockaddr_t *addr, socklen_t len, mn_timeout_t* tout)
{
    int err;
    *sent = 0;
    if (*sock == MN_SOCKET_INVALID) return MN_CLOSED;
    for ( ;; ) {
        long put = (long) sendto(*sock, data, count, 0, addr, len);  
        if (put > 0) {
            *sent = put;
            return MN_DONE;
        }
        err = errno;
        if (put == 0 || err == EPIPE) return MN_CLOSED;
        if (err == EINTR) continue;
        if (err != EAGAIN) return err;
        if ((err = mn_socket_waitfd(sock, WAITFD_W, tout)) != MN_DONE) return err;
    }
    return MN_UNKNOWN;
}

/**
 * Receive with timeout
 */
int mn_socket_recv(mn_socket_t* sock, char *data, size_t count, size_t *got, mn_timeout_t* tout) {
    int err;
    *got = 0;
    if (*sock == MN_SOCKET_INVALID) return MN_CLOSED;
    for ( ;; ) {
        long taken = (long) recv(*sock, data, count, 0);
        if (taken > 0) {
            *got = taken;
            return MN_DONE;
        }
        err = errno;
        if (taken == 0) return MN_CLOSED;
        if (err == EINTR) continue;
        if (err != EAGAIN) return err; 
        if ((err = mn_socket_waitfd(sock, WAITFD_R, tout)) != MN_DONE) return err; 
    }
    return MN_UNKNOWN;
}

/**
 * Recvfrom with timeout
 */
int mn_socket_recvfrom(mn_socket_t* sock, char *data, size_t count, size_t *got, 
        sockaddr_t *addr, socklen_t *len, mn_timeout_t* tout) {
    int err;
    *got = 0;
    if (*sock == MN_SOCKET_INVALID) return MN_CLOSED;
    for ( ;; ) {
        long taken = (long) recvfrom(*sock, data, count, 0, addr, len);
        if (taken > 0) {
            *got = taken;
            return MN_DONE;
        }
        err = errno;
        if (taken == 0) return MN_CLOSED;
        if (err == EINTR) continue;
        if (err != EAGAIN) return err; 
        if ((err = mn_socket_waitfd(sock, WAITFD_R, tout)) != MN_DONE) return err; 
    }
    return MN_UNKNOWN;
}

/**
 * Put socket into blocking mode
 */
void mn_socket_setblocking(mn_socket_t* sock) {
    int flags = fcntl(*sock, F_GETFL, 0);
    flags &= (~(O_NONBLOCK));
    fcntl(*sock, F_SETFL, flags);
}

/**
 * Put socket into non-blocking mode
 */
void mn_socket_setnonblocking(mn_socket_t* sock) {
    int flags = fcntl(*sock, F_GETFL, 0);
    flags |= O_NONBLOCK;
    fcntl(*sock, F_SETFL, flags);
}

/**
 * Select with timeout control
 */
int mn_select(int n, fd_set *rfds, fd_set *wfds, fd_set *efds, mn_timeout_t* tout) {
    int ret;
    do {
        struct timeval tv;
        double t = mn_timeout_getretry(tout);
        tv.tv_sec = (int) t;
        tv.tv_usec = (int) ((t - tv.tv_sec) * 1.0e6);
        /* timeout = 0 means no wait */
        ret = select(n, rfds, wfds, efds, t >= 0.0 ? &tv: NULL);
    } while (ret < 0 && errno == EINTR);
    return ret;
}

/**
 * Get host by address. 
 */
int mn_gethostbyaddr(const char *addr, socklen_t len, struct hostent **hp) {
    *hp = gethostbyaddr(addr, len, AF_INET);
    if (*hp) return MN_DONE;
    else if (h_errno) return h_errno;
    else if (errno) return errno;
    else return MN_UNKNOWN;
}

/**
 * Get host by name. 
 */
int mn_gethostbyname(const char *addr, struct hostent **hp) {
    *hp = gethostbyname(addr);
    if (*hp) return MN_DONE;
    else if (h_errno) return h_errno;
    else if (errno) return errno;
    else return MN_UNKNOWN;
}

/**
 * Error translation functions
 * Make sure important error messages are standard
 */
const char *mn_hoststrerror(int err) {
    if (err <= 0) return mn_strerror(err);
    switch (err) {
        case HOST_NOT_FOUND: return "host not found";
        default: return hstrerror(err);
    }
}

const char *mn_strerror(int err) {
    if (err <= 0) return mn_strerror(err);
    switch (err) {
        case EADDRINUSE: return "address already in use";
        case EISCONN: return "already connected";
        case EACCES: return "permission denied";
        case ECONNREFUSED: return "connection refused";
        case ECONNABORTED: return "closed";
        case ECONNRESET: return "closed";
        case ETIMEDOUT: return "timeout";
        default: return mn_strerror(errno);
    }
}

const char *mn_socket_ioerror(mn_socket_t* sock, int err) {
    (void) sock;
    return mn_strerror(err);
} 
#endif

/** @} */
