#ifndef MC_BUFFER_H
#define MC_BUFFER_H

/**
 * @file
 * @defgroup buffer CoAP Buffer
 * @{
 */

#include "msys/ms_config.h"

typedef struct mc_buffer mc_buffer_t;
struct mc_buffer {
    uint32_t nbytes;
    uint8_t* bytes;
};

mc_buffer_t* mc_buffer_alloc();
mc_buffer_t* mc_buffer_deinit(mc_buffer_t* buffer);
mc_buffer_t* mc_buffer_init(mc_buffer_t* buffer, uint32_t nbytes, uint8_t* bytes);

/** @} */

#endif
