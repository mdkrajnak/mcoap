#ifndef MC_HEADER_H
#define MC_HEADER_H

/** 
 * @file
 * @defgroup header CoAP Header
 * @{
 */

#include "msys/ms_config.h"

uint32_t mc_header_create(uint8_t version, uint8_t message_type, uint32_t token_len, uint8_t code, uint16_t message_id);
uint8_t mc_header_get_version(uint32_t header);
uint8_t mc_header_get_message_type(uint32_t header);
uint8_t mc_header_get_token_length(uint32_t header);
uint8_t mc_header_get_code(uint32_t header);
uint16_t mc_header_get_message_id(uint32_t header);

/** @} */

#endif
