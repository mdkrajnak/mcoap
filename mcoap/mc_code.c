/**
 * @file
 * @ingroup code
 * @{
 */
 
#include "mcoap/mc_code.h"

#define MC_CODE_CATEGORY_MASK 0xE0
#define MC_CODE_DETAIL_MASK   0x1F

/**
 * Given a category and detail create a CoAP code.
 * Note that category and detail are not checked to see if they are valid.
 */
uint8_t mc_code_create(uint8_t category, uint8_t detail) {
    uint8_t code = MC_CODE_CATEGORY_MASK & (category << 5);
    code = code | (MC_CODE_DETAIL_MASK & detail);
    return code;
}

/**
 * Given a code get the category.
 */
uint8_t mc_code_get_category(uint8_t code) {
    return (MC_CODE_CATEGORY_MASK & code) >> 5;
}

/** 
 * Given a code get the detail.
 */
uint8_t mc_code_get_detail(uint8_t code) {
    return (MC_CODE_DETAIL_MASK & code);
}

/** @} */
