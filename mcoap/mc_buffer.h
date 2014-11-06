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
uint8_t mc_buffer_next_uint8(const mc_buffer_t* buffer, uint32_t* bpos);
uint16_t mc_buffer_next_uint16(const mc_buffer_t* buffer, uint32_t* bpos);
uint32_t mc_buffer_next_uint32(const mc_buffer_t* buffer, uint32_t* bpos);
uint8_t* mc_buffer_next_ptr(const mc_buffer_t* buffer, uint32_t len, uint32_t* bpos);
mc_buffer_t* mc_buffer_copy_to(mc_buffer_t* buffer, uint32_t len, uint32_t* dpos, const mc_buffer_t* src, uint32_t* spos);
mc_buffer_t* mc_buffer_copy(const mc_buffer_t* src, uint32_t len, uint32_t* bpos);

/** @} */

#endif
