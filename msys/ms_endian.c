/**
 * @file
 * @ingroup endian 
 * @{
 * Portable routines for swapping data.  Standard net to host and host to net routines
 * are preferred for swapping 16 and 32 bit values.  Due to the lack of a standard for 
 * 64 bit values the 64 bit routines in this file should be used.
 *
 * The majority of these algorithms come from on online article:
 * http://www.lainoox.com/checking-endianness-of-your-operating-system/ 
 *
 * Including the very useful function for detecting platform byte order.
 */

#include <stdlib.h>
#include <stdio.h>

#include "msys/ms_endian.h"

/**
 * Platform independent way of detecting byte order.
 */
int ms_is_littleendian() {
    int x = 1;
    int result;
 
    if (*(char *)&x == 1)
        result = 1;
    else
        result = 0;
 
    return result;
}

/**
 * Platform independent way of swapping 16 bits.
 * When possible use ntoh/hton instead.
 */
uint16_t ms_swap_u16(uint16_t i) {
    uint8_t c1, c2;
 
    c1 = (uint8_t)(i & 0xFF);
    c2 = (uint8_t)((i >> 8) & 0xFF);
 
    return (uint16_t)((c1 << 8) + c2);
}

/**
 * Platform independent way of swapping 32 bits.
 * When possible use ntoh/hton instead.
 */
uint32_t ms_swap_u32(uint32_t i) {
    uint8_t c1, c2, c3, c4;    
 
    c1 = (uint8_t)(i & 0xFF);
    c2 = (uint8_t)((i >> 8) & 0xFF);
    c3 = (uint8_t)((i >> 16) & 0xFF);
    c4 = (uint8_t)((i >> 24) & 0xFF);
 
    return ((uint32_t)c1 << 24) + ((uint32_t)c2 << 16) + ((uint32_t)c3 << 8) + c4;
}

/**
 * Platform independent way of swapping 32 bits.
 * 64 bit versions of ntoh/hton exist but are rarely portable.
 */
uint64_t ms_swap_u64(uint64_t i) {
    uint8_t c1, c2, c3, c4, c5, c6, c7, c8; 
 
    c1 = (uint8_t)(i & 0xFF);
    c2 = (uint8_t)((i >> 8) & 0xFF);
    c3 = (uint8_t)((i >> 16) & 0xFF);
    c4 = (uint8_t)((i >> 24) & 0xFF);
    c5 = (uint8_t)((i >> 32) & 0xFF);
    c6 = (uint8_t)((i >> 40) & 0xFF);
    c7 = (uint8_t)((i >> 48) & 0xFF);
    c8 = (uint8_t)((i >> 56) & 0xFF);
 
    return ((uint64_t)c1 << 56) + 
            ((uint64_t)c2 << 48) + 
            ((uint64_t)c3 << 40) + 
            ((uint64_t)c4 << 32) + 
            ((uint64_t)c5 << 24) + 
            ((uint64_t)c6 << 16) + 
            ((uint64_t)c7 << 8) + 
            c8;
}

/**
 * Swap 64 bit values to network byte order in place.
 */
void ms_hton64(uint64_t* value) {
    if (ms_is_littleendian()) {
        *value = ms_swap_u64(*value);
    }
}
/**
 * Swap 64 bit values to host byte order in place.
 */
void ms_ntoh64(uint64_t* value) {
    if (ms_is_littleendian()) {
        *value = ms_swap_u64(*value);
    }
}

/** @} */
