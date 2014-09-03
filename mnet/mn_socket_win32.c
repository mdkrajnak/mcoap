/** 
 * @file
 * @ingroup socket
 * @{
 */

/**
 * Socket module for Win32.
 */
#ifdef _WIN32
#include <string.h>

#include "mnet/mn_socket.h"

/**
 * strerror implemention for winsock.
 */
static const char* wstrerror(int err) {
    switch (err) {
        case WSAEINTR: return "Interrupted function call";
        case WSAEACCES: return "Permission denied";
        case WSAEFAULT: return "Bad address";
        case WSAEINVAL: return "Invalid argument";
        case WSAEMFILE: return "Too many open files";
        case WSAEWOULDBLOCK: return "Resource temporarily unavailable";
        case WSAEINPROGRESS: return "Operation now in progress";
        case WSAEALREADY: return "Operation already in progress";
        case WSAENOTSOCK: return "Socket operation on nonsocket";
        case WSAEDESTADDRREQ: return "Destination address required";
        case WSAEMSGSIZE: return "Message too long";
        case WSAEPROTOTYPE: return "Protocol wrong type for socket";
        case WSAENOPROTOOPT: return "Bad protocol option";
        case WSAEPROTONOSUPPORT: return "Protocol not supported";
        case WSAESOCKTNOSUPPORT: return "Socket type not supported";
        case WSAEOPNOTSUPP: return "Operation not supported";
        case WSAEPFNOSUPPORT: return "Protocol family not supported";
        case WSAEAFNOSUPPORT: 
            return "Address family not supported by protocol family"; 
        case WSAEADDRINUSE: return "Address already in use";
        case WSAEADDRNOTAVAIL: return "Cannot assign requested address";
        case WSAENETDOWN: return "Network is down";
        case WSAENETUNREACH: return "Network is unreachable";
        case WSAENETRESET: return "Network dropped connection on reset";
        case WSAECONNABORTED: return "Software caused connection abort";
        case WSAECONNRESET: return "Connection reset by peer";
        case WSAENOBUFS: return "No buffer space available";
        case WSAEISCONN: return "Socket is already connected";
        case WSAENOTCONN: return "Socket is not connected";
        case WSAESHUTDOWN: return "Cannot send after socket shutdown";
        case WSAETIMEDOUT: return "Connection timed out";
        case WSAECONNREFUSED: return "Connection refused";
        case WSAEHOSTDOWN: return "Host is down";
        case WSAEHOSTUNREACH: return "No route to host";
        case WSAEPROCLIM: return "Too many processes";
        case WSASYSNOTREADY: return "Network subsystem is unavailable";
        case WSAVERNOTSUPPORTED: return "Winsock.dll version out of range";
        case WSANOTINITIALISED: 
            return "Successful WSAStartup not yet performed";
        case WSAEDISCON: return "Graceful shutdown in progress";
        case WSAHOST_NOT_FOUND: return "Host not found";
        case WSATRY_AGAIN: return "Nonauthoritative host not found";
        case WSANO_RECOVERY: return "Nonrecoverable name lookup error"; 
        case WSANO_DATA: return "Valid name, no data record of requested type";
        default: return "Unknown error";
    }
}
/**
 * Initializes module 
 */
int mn_socket_open() {
    WSADATA wsaData;
    WORD wVersionRequested = MAKEWORD(2, 0); 
    int err = WSAStartup(wVersionRequested, &wsaData );
    if (err != 0) return 0;
    if ((LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 0) &&
        (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1)) {
        WSACleanup();
        return 0; 
    }
    return US_DONE;
}

/**
 * Close module 
 */
int mn_socket_close() {
    WSACleanup();
    return 1;
}

/**
 * Wait for readable/writable/connected socket with timeout
 */
#define WAITFD_R        1
#define WAITFD_W        2
#define WAITFD_E        4
#define WAITFD_C        (WAITFD_E|WAITFD_W)

int mn_socket_waitfd(mn_socket_t* sock, int sw, us_timeout_t* tout) {
    int ret;
    fd_set rfds, wfds, efds, *rp = NULL, *wp = NULL, *ep = NULL;
    struct timeval tv, *tp = NULL;
    double t;
    if (timeout_iszero(tm)) return UN_TIMEOUT;  /* optimize timeout == 0 case */
    if (sw & WAITFD_R) { 
        FD_ZERO(&rfds); 
        FD_SET(*sock, &rfds);
        rp = &rfds; 
    }
    if (sw & WAITFD_W) { FD_ZERO(&wfds); FD_SET(*sock, &wfds); wp = &wfds; }
    if (sw & WAITFD_C) { FD_ZERO(&efds); FD_SET(*sock, &efds); ep = &efds; }
    if ((t = timeout_get(tm)) >= 0.0) {
        tv.tv_sec = (int) t;
        tv.tv_usec = (int) ((t-tv.tv_sec)*1.0e6);
        tp = &tv;
    }
    ret = select(0, rp, wp, ep, tp);
    if (ret == -1) return WSAGetLastError();
    if (ret == 0) return UN_TIMEOUT;
    if (sw == WAITFD_C && FD_ISSET(*sock, &efds)) return UN_CLOSED;
    return UN_DONE;
}

/**
 * Close and destroy socket
 */
void mn_socket_destroy(mn_socket_t* sock) {
    if (*sock != UN_SOCKET_INVALID) {
        socket_setblocking(ps); /* close can take a long time on WIN32 */
        closesocket(*sock);
        *sock = UN_SOCKET_INVALID;
    }
}

/**
 * 
 */
void mn_socket_shutdown(mn_socket_t* sock, int how) {
    mn_socket_setblocking(ps);
    shutdown(*sock, how);
    mn_socket_setnonblocking(ps);
}

/**
 * Creates and sets up a socket
 */
int mn_socket_create(mn_socket_t* sock, int domain, int type, int protocol) {
    *sock = socket(domain, type, protocol);
    if (*sock != UN_SOCKET_INVALID) return UN_DONE;
    else return WSAGetLastError();
}

/**
 * Connects or returns error message
 */
int mn_socket_connect(mn_socket_t* sock, sockaddr_t* addr, socklen_t len, us_timeout_t* tout) {
    int err;
    
    /* don't call on closed socket */
    
    if (*sock == UN_SOCKET_INVALID) return UN_CLOSED;
    
    /* ask system to connect */
    if (connect(*sock, addr, len) == 0) return UN_DONE;
    
    /* make sure the system is trying to connect */
    err = WSAGetLastError();
    if (err != WSAEWOULDBLOCK && err != WSAEINPROGRESS) return err;
    
    /* zero timeout case optimization */
    if (mn_timeout_iszero(tm)) return UN_TIMEOUT;
    
    /* we wait until something happens */
    err = mn_socket_waitfd(ps, WAITFD_C, tm);
    if (err == UN_CLOSED) {
        int elen = sizeof(err);
        
        /* give windows time to set the error (yes, disgusting) */
        Sleep(10);
        
        /* find out why we failed */
        getsockopt(*sock, SOL_SOCKET, SO_ERROR, (char *)&err, &elen); 
        
        /* we KNOW there was an error. if 'why' is 0, we will return
        * "unknown error", but it's not really our fault */
        return err > 0? err: UN_UNKNOWN; 
    } else return err;
}

/**
 * Binds or returns error message
 */
int mn_socket_bind(mn_socket_t* sock, sockaddr_t* addr, socklen_t len) {
    int err = UN_DONE;
    mn_socket_setblocking(ps);
    if (bind(*sock, addr, len) < 0) err = WSAGetLastError();
    mn_socket_setnonblocking(ps);
    return err;
}

/**
 * Tell a socket to listen. 
 */
int mn_socket_listen(mn_socket_t* sock, int backlog) {
    int err = UN_DONE;
    mn_socket_setblocking(ps);
    if (listen(*sock, backlog) < 0) err = WSAGetLastError();
    mn_socket_setnonblocking(ps);
    return err;
}

/**
 * Accept with timeout
 */
int mn_socket_accept(mn_socket_t* sock, mn_socket_t* asock, sockaddr_t* addr, socklen_t *len, us_timeout_t* tout) {
    SA daddr;
    socklen_t dlen = sizeof(daddr);
    if (*sock == UN_SOCKET_INVALID) return UN_CLOSED;
    if (!addr) addr = &daddr;
    if (!len) len = &dlen;
    for ( ;; ) {
        int err;
        
        /* try to get client socket */
        if ((*asock = accept(*sock, addr, len)) != UN_SOCKET_INVALID) return UN_DONE;
        
        /* find out why we failed */
        err = WSAGetLastError(); 
        
        /* if we failed because there was no connectoin, keep trying */
        if (err != WSAEWOULDBLOCK && err != WSAECONNABORTED) return err;
        
        /* call select to avoid busy wait */
        if ((err = mn_socket_waitfd(asock, WAITFD_R, tout)) != UN_DONE) return err;
    } 
    /* can't reach here */
    //return UN_UNKNOWN; 
}

/**
 * Send with timeout
 * On windows, if you try to send 10MB, the OS will buffer EVERYTHING 
 * this can take an awful lot of time and we will end up blocked. 
 * Therefore, whoever calls this function should not pass a huge buffer.
 */
int mn_socket_send(mn_socket_t* sock, const char* data, size_t count, 
        size_t* sent, us_timeout_t* tout)
{
    int err;
    *sent = 0;
    
    /* avoid making system calls on closed sockets */
    if (*sock == UN_SOCKET_INVALID) return UN_CLOSED;
    /* loop until we send something or we give up on error */
    for ( ;; ) {
        /* try to send something */
        int put = send(*sock, data, (int) count, 0);
        
        /* if we sent something, we are done */
        if (put > 0) {
            *sent = put;
            return UN_DONE;
        }
        
        /* deal with failure */
        err = WSAGetLastError(); 
        
        /* we can only proceed if there was no serious error */
        if (err != WSAEWOULDBLOCK) return err;
        
        /* avoid busy wait */
        if ((err = mn_socket_waitfd(ps, WAITFD_W, tm)) != UN_DONE) return err;
    } 
    /* can't reach here */
    // return UN_UNKNOWN;
}

/**
 * Sendto with timeout
 */
int mn_socket_sendto(mn_socket_t* sock, const char* data, size_t count, size_t* sent, 
        sockaddr_t* addr, socklen_t len, us_timeout_t* tout)
{
    int err;
    *sent = 0;
    if (*sock == UN_SOCKET_INVALID) return UN_CLOSED;
    for ( ;; ) {
        int put = sendto(*sock, data, (int) count, 0, addr, len);
        if (put > 0) {
            *sent = put;
            return UN_DONE;
        }
        err = WSAGetLastError(); 
        if (err != WSAEWOULDBLOCK) return err;
        if ((err = mn_socket_waitfd(ps, WAITFD_W, tm)) != UN_DONE) return err;
    } 
    // return UN_UNKNOWN;
}

/**
 * Receive with timeout
 */
int mn_socket_recv(mn_socket_t* sock, char* data, size_t count, size_t* got, us_timeout_t* tout) {
    int err;
    *got = 0;
    if (*sock == UN_SOCKET_INVALID) return UN_CLOSED;
    for ( ;; ) {
        int taken = recv(*sock, data, (int) count, 0);
        if (taken > 0) {
            *got = taken;
            return UN_DONE;
        }
        if (taken == 0) return UN_CLOSED;
        err = WSAGetLastError();
        if (err != WSAEWOULDBLOCK) return err;

        /* Socket is in a state where it would block, wait with timeout until its ready. */
        if ((err = mn_socket_waitfd(ps, WAITFD_R, tm)) != UN_DONE) return err;
    }
    // return UN_UNKNOWN;
}

/**
 * Recvfrom with timeout
 */
int mn_socket_recvfrom(mn_socket_t* sock, char* data, size_t count, size_t* got, 
        sockaddr_t* addr, socklen_t* len, us_timeout_t* tout) {
    int err;
    *got = 0;
    if (*sock == UN_SOCKET_INVALID) return UN_CLOSED;
    for ( ;; ) {
        int taken = recvfrom(*sock, data, (int) count, 0, addr, len);
        if (taken > 0) {
            *got = taken;
            return UN_DONE;
        }
        if (taken == 0) return UN_CLOSED;
        err = WSAGetLastError();
        if (err != WSAEWOULDBLOCK) return err;
        if ((err = mn_socket_waitfd(ps, WAITFD_R, tm)) != UN_DONE) return err;
    }
    // Unreachable code.
    // return UN_UNKNOWN;
}

/**
 * Put socket into blocking mode
 */
void mn_socket_setblocking(mn_socket_t* sock) {
    u_long argp = 0;
    ioctlsocket(*sock, FIONBIO, &argp);
}

/**
 * Put socket into non-blocking mode
 */
void mn_socket_setnonblocking(mn_socket_t* sock) {
    u_long argp = 1;
    ioctlsocket(*sock, FIONBIO, &argp);
}

/**
 * Select with int timeout in ms
 */
int mn_select(int n, fd_set *rfds, fd_set *wfds, fd_set *efds, mn_timeout_t* tm) {
    struct timeval tv; 
    double t = timeout_get(tm);
    tv.tv_sec = (int) t;
    tv.tv_usec = (int) ((t - tv.tv_sec) * 1.0e6);
    if (n <= 0) {
        Sleep((DWORD) (1000*t));
        return 0;
    } else return select(0, rfds, wfds, efds, t >= 0.0? &tv: NULL);
}

/**
 * DNS helpers 
 */
int mn_gethostbyaddr(const char* addr, socklen_t len, hostent_t** hp) {
    *hp = gethostbyaddr(addr, len, AF_INET);
    if (*hp) return UN_DONE;
    else return WSAGetLastError();
}

int mn_gethostbyname(const char* addr, hostent_t** hp) {
    *hp = gethostbyname(addr);
    if (*hp) return UN_DONE;
    else return  WSAGetLastError();
}

/**
 * Error translation functions
 */
const char* mn_hoststrerror(int err) {
    if (err <= 0) return UN_strerror(err);
    switch (err) {
        case WSAHOST_NOT_FOUND: return "host not found";
        default: return wstrerror(err); 
    }
}

const char* mn_strerror(int err) {
    if (err <= 0) return UN_strerror(err);
    switch (err) {
        case WSAEADDRINUSE: return "address already in use";
        case WSAECONNREFUSED: return "connection refused";
        case WSAEISCONN: return "already connected";
        case WSAEACCES: return "permission denied";
        case WSAECONNABORTED: return "closed";
        case WSAECONNRESET: return "closed";
        case WSAETIMEDOUT: return "timeout";
        default: return wstrerror(err);
    }
}

const char* mn_ioerror(mn_socket_t* sock, int err) {
    (void) ps;
    return mn_strerror(err);
}

#endif

/** @} */
