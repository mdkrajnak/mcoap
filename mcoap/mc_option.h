#ifndef MC_OPTION_H
#define MC_OPTION_H

/**
 * @file
 * @defgroup option CoAP Option
 * @{
 */

#include "mcoap/mc_buffer.h"

/** In memory (vs on-the-wire) option value. */
typedef struct mc_option mc_option_t;
struct mc_option {
    uint16_t option_num;
    mc_buffer_t value;
};

mc_option_t* mc_option_alloc();
mc_option_t* mc_option_nalloc(uint32_t count);
mc_option_t* mc_option_deinit(mc_option_t* option);
mc_option_t* mc_option_ndeinit(mc_option_t* option, uint32_t count);
mc_option_t* mc_option_init(mc_option_t* option, uint16_t option_num, uint32_t nbytes, uint8_t* bytes);
mc_option_t* mc_option_init_uint32(mc_option_t* option, uint16_t option_num, uint32_t value);
uint32_t mc_option_as_uint32(mc_option_t* option);
uint32_t mc_option_buffer_size(mc_option_t* option, uint32_t prev_option_num);

/** @} */
#endif
