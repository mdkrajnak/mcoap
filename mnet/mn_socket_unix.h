#ifndef MN_SOCKET_UNIX_H
#define MN_SOCKET_UNIX_H

/**
 * @file
 * @ingroup socket
 * @{
 */

/**
 * Socket module for Unix
 */

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

typedef int mn_socket_t;

#define MN_SOCKET_INVALID (-1)

/** @} */

#endif
