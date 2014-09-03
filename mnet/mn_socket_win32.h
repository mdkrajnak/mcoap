#ifndef MN_SOCKET_WIN32_H
#define MN_SOCKET_WIN32_H

/**
 * @file
 * @ingroup socket
 * @{
 */

/* Socket module for Win32 */

#include <winsock.h>

typedef int socklen_t;
typedef SOCKET mn_socket_t;

#define MN_SOCKET_INVALID (INVALID_SOCKET)

/** @} */

#endif 
