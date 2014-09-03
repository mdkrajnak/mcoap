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

uint32_t mc_options_list_buffer_size(mc_options_list_t* list);
mc_buffer_t* mc_options_list_to_buffer(mc_options_list_t* list, mc_buffer_t* buffer);
mc_buffer_t* mc_options_list_mk_buffer(mc_options_list_t* list);

/** @} */

#endif
