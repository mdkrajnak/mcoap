#ifndef MN_SOCKADDR_H
#define MN_SOCKADDR_H

/** 
 * @file
 * @ingroup socket
 * @{
 */

#include <stdio.h>
#include "mnet/mn_socket.h"

sockaddr_t* mn_sockaddr_alloc();
sockaddr_t* mn_sockaddr_copy(sockaddr_t* src);
sockaddr_t* mn_sockaddr_inet_init(sockaddr_t* addr, const char *hostname, unsigned short port);

/** @} */

#endif

