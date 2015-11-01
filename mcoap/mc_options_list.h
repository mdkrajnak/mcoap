#ifndef MC_OPTIONS_LIST_H
#define MC_OPTIONS_LIST_H

/**
 * @file
 * @defgroup options_list CoAP Options List
 * @{
 */

#include "mcoap/mc_option.h"

/** In memory (vs on-the-wire) option list. */
typedef struct mc_options_list mc_options_list_t;
struct mc_options_list {
    uint32_t noptions;
    mc_option_t* options;
};

mc_options_list_t* mc_options_list_alloc();
mc_options_list_t* mc_options_list_deinit(mc_options_list_t* list);
mc_options_list_t* mc_options_list_init(mc_options_list_t* list, uint32_t noptions, mc_option_t* options);
mc_options_list_t* mc_options_list_vinit(mc_options_list_t* list, uint32_t noptions, ...);
mc_options_list_t* mc_options_list_copy(mc_options_list_t* list);
mc_options_list_t* mc_options_list_merge(mc_options_list_t* list1, mc_options_list_t* list2);
mc_options_list_t* mc_options_list_from_buffer(mc_options_list_t* list, mc_buffer_t* buffer, uint32_t* bpos);
int mc_options_list_get_index(mc_options_list_t* list, uint32_t start, uint16_t optnum);
mc_option_t* mc_options_list_get(mc_options_list_t* list, int index);

uint32_t mc_options_list_buffer_size(const mc_options_list_t* list);
mc_buffer_t* mc_options_list_to_buffer(const mc_options_list_t* list, mc_buffer_t* buffer, uint32_t* bpos);

/** @} */

#endif
