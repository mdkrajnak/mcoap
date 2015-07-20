#ifndef MC_CODE_H
#define MC_CODE_H

/** 
 * @file
 * @defgroup code CoAP Code
 * @{
 */

#include "msys/ms_config.h"

#define MC_CODE_REQUEST	 0
#define MC_CODE_RESPONSE 2

#define MC_GET		1
#define MC_POST		2
#define MC_PUT		3
#define MC_DELETE	4

uint8_t mc_code_create(uint8_t category, uint8_t detail);
uint8_t mc_code_get_category(uint8_t code);
uint8_t mc_code_get_detail(uint8_t code);


/** @} */

#endif
