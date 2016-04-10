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
#include "mnet/mn_socket_win32.h"
#include <inaddr.h>
#include <in6addr.h>
#include <ws2ipdef.h>



 /**
 * From github user dubcanada at:
 * https://github.com/dubcanada/inet_pton
 *
 * int inet_pton(int af, const char *src, void *dst);
 *
 * Compatible with inet_pton just replace inet_pton with xp_inet_pton
 * and you are good to go. Uses WSAStringToAddressA instead of
 * inet_pton, compatible with Windows 2000 +
 */
int inet_pton(int family, const char *src, void *dst)
{
	int rc;
	struct sockaddr_storage addr;
	int addr_len = sizeof(addr);

	addr.ss_family = family;

	rc = WSAStringToAddressA((char *)src, family, NULL,
		(struct sockaddr*) &addr, &addr_len);
	if (rc != 0) {
		return -1;
	}

	if (family == AF_INET) {
		memcpy(dst, &((struct sockaddr_in *) &addr)->sin_addr,
			sizeof(struct in_addr));
	}
	else if (family == AF_INET6) {
		memcpy(dst, &((struct sockaddr_in6 *)&addr)->sin6_addr,
			sizeof(struct in6_addr));
	}

	return 1;
}

/**
 * This is a minimal implementation of inet_aton since its not available on windows.
 * Convert IPv4 decimal dotted IP address into an IP number.
 * @return 0 on failure, not 0 on success.
 */
int inet_aton(const char* cp, inetaddr_t* inp) {
    unsigned int a = 0, b = 0, c = 0, d = 0;
    int n = 0, r;
    unsigned long int addr = 0;

    /* Restrict the width to prevent format string attacks but leave it a */
    /* little longer then we need to detect large numbers and allow */
    /* preceeding 0's. */
    r = sscanf(cp, "%5u.%5u.%5u.%5u%n", &a, &b, &c, &d, &n);
    if (r == 0 || n == 0) return 0;
    cp += n;

    /* If the next character is not a null terminator, or */
    /* Any value read is greater then 255, return 0 (error) */

    if (*cp) return 0;
    if (a > 255 || b > 255 || c > 255 || d > 255) return 0;
    if (inp) {
        addr += a; addr <<= 8;
        addr += b; addr <<= 8;
        addr += c; addr <<= 8;
        addr += d;
        inp->sin_addr.S_un.S_addr = htonl(addr);
    }
    return 1;
}

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
    return MN_DONE;
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

int mn_socket_waitfd(mn_socket_t* sock, int sw, mn_timeout_t* tout) {
    int ret;
    fd_set rfds, wfds, efds, *rp = NULL, *wp = NULL, *ep = NULL;
    struct timeval tv, *tp = NULL;
    double t;
    if (mn_timeout_iszero(tout)) return MN_TIMEOUT;  /* optimize timeout == 0 case */
    if (sw & WAITFD_R) { 
        FD_ZERO(&rfds); 
        FD_SET(*sock, &rfds);
        rp = &rfds; 
    }
    if (sw & WAITFD_W) { FD_ZERO(&wfds); FD_SET(*sock, &wfds); wp = &wfds; }
    if (sw & WAITFD_C) { FD_ZERO(&efds); FD_SET(*sock, &efds); ep = &efds; }
    if ((t = mn_timeout_get(tout)) >= 0.0) {
        tv.tv_sec = (int) t;
        tv.tv_usec = (int) ((t-tv.tv_sec)*1.0e6);
        tp = &tv;
    }
    ret = select(0, rp, wp, ep, tp);
    if (ret == -1) return WSAGetLastError();
    if (ret == 0) return MN_TIMEOUT;
    if (sw == WAITFD_C && FD_ISSET(*sock, &efds)) return MN_CLOSED;
    return MN_DONE;
}

/**
 * Close and destroy socket
 */
int mn_socket_destroy(mn_socket_t* sock) {
    if (*sock != MN_SOCKET_INVALID) {
        mn_socket_setblocking(sock); /* close can take a long time on WIN32 */
        closesocket(*sock);
        *sock = MN_SOCKET_INVALID;
    }
    return MN_DONE;
}

/**
 * 
 */
void mn_socket_shutdown(mn_socket_t* sock, int how) {
    mn_socket_setblocking(sock);
    shutdown(*sock, how);
    mn_socket_setnonblocking(sock);
}

/**
 * Creates and sets up a socket
 */
int mn_socket_create(mn_socket_t* sock, int domain, int type, int protocol) {
    *sock = socket(domain, type, protocol);
    if (*sock != MN_SOCKET_INVALID) return MN_DONE;
    else return WSAGetLastError();
}

/**
 * Connects or returns error message
 */
int mn_socket_connect(mn_socket_t* sock, sockaddr_t* addr, socklen_t len, mn_timeout_t* tout) {
    int err;
    
    /* don't call on closed socket */
    
    if (*sock == MN_SOCKET_INVALID) return MN_CLOSED;
    
    /* ask system to connect */
    if (connect(*sock, addr, len) == 0) return MN_DONE;
    
    /* make sure the system is trying to connect */
    err = WSAGetLastError();
    if (err != WSAEWOULDBLOCK && err != WSAEINPROGRESS) return err;
    
    /* zero timeout case optimization */
    if (mn_timeout_iszero(tout)) return MN_TIMEOUT;
    
    /* we wait until something happens */
    err = mn_socket_waitfd(sock, WAITFD_C, tout);
    if (err == MN_CLOSED) {
        int elen = sizeof(err);
        
        /* give windows time to set the error (yes, disgusting) */
        Sleep(10);
        
        /* find out why we failed */
        getsockopt(*sock, SOL_SOCKET, SO_ERROR, (char *)&err, &elen); 
        
        /* we KNOW there was an error. if 'why' is 0, we will return
        * "unknown error", but it's not really our fault */
        return err > 0? err: MN_UNKNOWN; 
    } else return err;
}

/**
 * Binds or returns error message
 */
int mn_socket_bind(mn_socket_t* sock, sockaddr_t* addr, socklen_t len) {
    int err = MN_DONE;
    mn_socket_setblocking(sock);
    if (bind(*sock, addr, len) < 0) err = WSAGetLastError();
    mn_socket_setnonblocking(sock);
    return err;
}

/**
 * Tell a socket to listen. 
 */
int mn_socket_listen(mn_socket_t* sock, int backlog) {
    int err = MN_DONE;
    mn_socket_setblocking(sock);
    if (listen(*sock, backlog) < 0) err = WSAGetLastError();
    mn_socket_setnonblocking(sock);
    return err;
}

/**
 * Accept with timeout
 */
int mn_socket_accept(mn_socket_t* sock, mn_socket_t* asock, sockaddr_t* addr, socklen_t *len, mn_timeout_t* tout) {
    sockaddr_t daddr;
    socklen_t dlen = sizeof(daddr);
    if (*sock == MN_SOCKET_INVALID) return MN_CLOSED;
    if (!addr) addr = &daddr;
    if (!len) len = &dlen;
    for ( ;; ) {
        int err;
        
        /* try to get client socket */
        if ((*asock = accept(*sock, addr, len)) != MN_SOCKET_INVALID) return MN_DONE;
        
        /* find out why we failed */
        err = WSAGetLastError(); 
        
        /* if we failed because there was no connectoin, keep trying */
        if (err != WSAEWOULDBLOCK && err != WSAECONNABORTED) return err;
        
        /* call select to avoid busy wait */
        if ((err = mn_socket_waitfd(asock, WAITFD_R, tout)) != MN_DONE) return err;
    } 
    /* can't reach here */
    //return MN_UNKNOWN; 
}

/**
 * Send with timeout
 * On windows, if you try to send 10MB, the OS will buffer EVERYTHING 
 * this can take an awful lot of time and we will end up blocked. 
 * Therefore, whoever calls this function should not pass a huge buffer.
 */
int mn_socket_send(mn_socket_t* sock, const char* data, size_t count, 
        size_t* sent, mn_timeout_t* tout)
{
    int err;
    *sent = 0;
    
    /* avoid making system calls on closed sockets */
    if (*sock == MN_SOCKET_INVALID) return MN_CLOSED;
    /* loop until we send something or we give up on error */
    for ( ;; ) {
        /* try to send something */
        int put = send(*sock, data, (int) count, 0);
        
        /* if we sent something, we are done */
        if (put > 0) {
            *sent = put;
            return MN_DONE;
        }
        
        /* deal with failure */
        err = WSAGetLastError(); 
        
        /* we can only proceed if there was no serious error */
        if (err != WSAEWOULDBLOCK) return err;
        
        /* avoid busy wait */
        if ((err = mn_socket_waitfd(sock, WAITFD_W, tout)) != MN_DONE) return err;
    } 
    /* can't reach here */
    // return MN_UNKNOWN;
}

/**
 * Sendto with timeout
 */
int mn_socket_sendto(mn_socket_t* sock, const char* data, size_t count, size_t* sent, 
        sockaddr_t* addr, socklen_t len, mn_timeout_t* tout)
{
    int err;
    *sent = 0;
    if (*sock == MN_SOCKET_INVALID) return MN_CLOSED;
    for ( ;; ) {
        int put = sendto(*sock, data, (int) count, 0, addr, len);
        if (put > 0) {
            *sent = put;
            return MN_DONE;
        }
        err = WSAGetLastError(); 
        if (err != WSAEWOULDBLOCK) return err;
        if ((err = mn_socket_waitfd(sock, WAITFD_W, tout)) != MN_DONE) return err;
    } 
    // return MN_UNKNOWN;
}

/**
 * Receive with timeout
 */
int mn_socket_recv(mn_socket_t* sock, char* data, size_t count, size_t* got, mn_timeout_t* tout) {
    int err;
    *got = 0;
    if (*sock == MN_SOCKET_INVALID) return MN_CLOSED;
    for ( ;; ) {
        int taken = recv(*sock, data, (int) count, 0);
        if (taken > 0) {
            *got = taken;
            return MN_DONE;
        }
        if (taken == 0) return MN_CLOSED;
        err = WSAGetLastError();
        if (err != WSAEWOULDBLOCK) return err;

        /* Socket is in a state where it would block, wait with timeout until its ready. */
        if ((err = mn_socket_waitfd(sock, WAITFD_R, tout)) != MN_DONE) return err;
    }
    // return MN_UNKNOWN;
}

/**
 * Recvfrom with timeout
 */
int mn_socket_recvfrom(mn_socket_t* sock, char* data, size_t count, size_t* got, 
        sockaddr_t* addr, socklen_t* len, mn_timeout_t* tout) {
    int err;
    *got = 0;
    if (*sock == MN_SOCKET_INVALID) return MN_CLOSED;
    for ( ;; ) {
        int taken = recvfrom(*sock, data, (int) count, 0, addr, len);
        if (taken > 0) {
            *got = taken;
            return MN_DONE;
        }
        if (taken == 0) return MN_CLOSED;
        err = WSAGetLastError();
        if (err != WSAEWOULDBLOCK) return err;
        if ((err = mn_socket_waitfd(sock, WAITFD_R, tout)) != MN_DONE) return err;
    }
    // Unreachable code.
    // return MN_UNKNOWN;
}

/**
 * Put socket into blocking mode
 */
int mn_socket_setblocking(mn_socket_t* sock) {
    u_long argp = 0;
    ioctlsocket(*sock, FIONBIO, &argp);
	return MN_DONE;
}

/**
 * Put socket into non-blocking mode
 */
int mn_socket_setnonblocking(mn_socket_t* sock) {
    u_long argp = 1;
    ioctlsocket(*sock, FIONBIO, &argp);
	return MN_DONE;
}

/**
 * Select with int timeout in ms
 */
int mn_select(int n, fd_set *rfds, fd_set *wfds, fd_set *efds, mn_timeout_t* tm) {
    struct timeval tv; 
    double t = mn_timeout_get(tm);
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
    if (*hp) return MN_DONE;
    else return WSAGetLastError();
}

int mn_gethostbyname(const char* addr, hostent_t** hp) {
    *hp = gethostbyname(addr);
    if (*hp) return MN_DONE;
    else return  WSAGetLastError();
}

/**
 * Error translation functions
 */
const char* mn_hoststrerror(int err) {
    if (err <= 0) return mn_strerror(err);
    switch (err) {
        case WSAHOST_NOT_FOUND: return "host not found";
        default: return wstrerror(err); 
    }
}

const char* mn_strerror(int err) {
    if (err <= 0) return mn_strerror(err);
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
    (void) sock;
    return mn_strerror(err);
}

#endif

/** @} */
