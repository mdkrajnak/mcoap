#ifndef MC_URI_H
#define MC_URI_H

/** 
 * @file
 * @defgroup uri CoAP URI
 * @{
 */

#include "msys/ms_config.h"
#include "mnet/mn_socket.h"
#include "mcoap/mc_options_list.h"

#define MC_DEFAULT_PORT 5683

mc_options_list_t* mc_uri_to_options(mc_options_list_t* const list, sockaddr_t* const dest, char* const uri);
sockaddr_t* mc_uri_to_address(sockaddr_t* const addr, char* const uri);

/** @} */

#endif
