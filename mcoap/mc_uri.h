#ifndef MC_URI_H
#define MC_URI_H

/** 
 * @file
 * @defgroup uri CoAP URI
 * @{
 */

#include "msys/ms_config.h"
#include "mcoap/mc_options_list.h"

#define MC_DEFAULT_PORT 5683

mc_options_list_t* mc_uri_parse(mc_options_list_t* list, char* const host, uint16_t port, char* const uri);

/** @} */

#endif
