#ifndef MC_TOKEN_H_
#define MC_TOKEN_H_

/**
 * @file
 * @defgroup mc_token.h mc_token.h
 * @{
 */

#include "mcoap/mc_buffer.h"

mc_buffer_t* mc_token_create1(uint8_t prefix);
mc_buffer_t* mc_token_create2(uint16_t prefix);
mc_buffer_t* mc_token_create4(uint32_t prefix);

/** @} */
#endif
