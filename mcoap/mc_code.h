#ifndef MC_CODE_H
#define MC_CODE_H

/** 
 * @file
 * @defgroup code CoAP Code
 * @{
 */

#include "msys/ms_config.h"

uint8_t mc_code_create(uint8_t category, uint8_t detail);
uint8_t mc_code_get_category(uint8_t code);
uint8_t mc_code_get_detail(uint8_t code);


/** @} */

#endif
