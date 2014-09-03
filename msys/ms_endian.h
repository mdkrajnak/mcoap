#ifndef MS_ENDIAN_H
#define MS_ENDIAN_H

/**
 * @file
 * @defgroup endian Byte Swapping Utilities 
 * @{
 * Utilities for byte swapping.  For 16 and 32 bit values the local
 * platform ntoh/hton are preferred.
 */
 
#include "msys/ms_config.h"

int ms_is_littleendian();

uint16_t ms_swap_u16(uint16_t i);
 
uint32_t ms_swap_u32(uint32_t i);
 
uint64_t ms_swap_u64(uint64_t i);

void ms_hton64(uint64_t* value);

void ms_ntoh64(uint64_t* value);

/** @} */

#endif
