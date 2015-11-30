#ifndef MN_SOCKET_WIN32_H
#define MN_SOCKET_WIN32_H

/**
 * @file
 * @ingroup socket
 * @{
 */

/* Socket module for Win32 */

#include <winsock2.h>

typedef int socklen_t;
typedef SOCKET mn_socket_t;

#define MN_SOCKET_INVALID (INVALID_SOCKET)

int inet_aton(const char *cp, struct sockaddr_in *inp);
int inet_pton(int family, const char *src, void *dst);

/** @} */

#endif 
