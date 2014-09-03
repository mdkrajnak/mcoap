#ifndef MN_SOCKET_H
#define MN_SOCKET_H

/**
 * @file
 * @defgroup socket Portable Socket Wrapper
 * @{
 */

#include "mnet/mn_error.h"

#ifdef _WIN32
#include "mnet/mn_socket_win32.h"
#else
#include "mnet/mn_socket_unix.h"
#endif

#include "mnet/mn_timeout.h"

/* Typedef sockaddr,hostent to an _t name. */
typedef struct sockaddr sockaddr_t;
typedef struct hostent hostent_t;

/* Define an abstact socket interface. */
int mn_socket_open();
int mn_socket_close();
void mn_socket_shutdown(mn_socket_t* sock, int how); 
void mn_socket_destroy(mn_socket_t* sock);

int mn_socket_sendto(
    mn_socket_t* sock, const char* data, size_t count, size_t* sent, 
    sockaddr_t* addr, socklen_t addr_len, mn_timeout_t* tout);

int mn_socket_recvfrom(
    mn_socket_t* sock, char* data, size_t count, size_t* got, 
    sockaddr_t* addr, socklen_t* addr_len, mn_timeout_t* tout);

void mn_socket_setnonblocking(mn_socket_t* sock);
void mn_socket_setblocking(mn_socket_t* sock);

int mn_socket_waitfd(mn_socket_t* sock, int sw, mn_timeout_t* tout);

int mn_socket_connect(mn_socket_t* sock, sockaddr_t* addr, socklen_t addr_len, mn_timeout_t* tout); 
int mn_socket_create(mn_socket_t* sock, int domain, int type, int protocol);
int mn_socket_bind(mn_socket_t* sock, sockaddr_t* addr, socklen_t addr_len); 
int mn_socket_listen(mn_socket_t* sock, int backlog);
int mn_socket_accept(mn_socket_t* sock, mn_socket_t* asock, sockaddr_t* addr, socklen_t* addr_len, mn_timeout_t* tout);

int mn_socket_send(mn_socket_t* sock, const char* data, size_t count, size_t* sent, mn_timeout_t* tout);
int mn_socket_recv(mn_socket_t* sock, char* data, size_t count, size_t* got, mn_timeout_t* tout);
const char* mn_socket_ioerror(mn_socket_t* sock, int err);

int mn_select(int nsock, fd_set* rfds, fd_set* wfds, fd_set* efds, mn_timeout_t* tout);
const char* mn_hoststrerror(int err);
const char* mn_strerror(int err);

int mn_gethostbyaddr(const char* addr, socklen_t len, hostent_t** hp);
int mn_gethostbyname(const char* addr, hostent_t** hp);

/** @} */

#endif
