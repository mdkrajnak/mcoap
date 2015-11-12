#ifndef MCOAP_PRINT_H
#define MCOAP_PRINT_H

#include "mcoap/mc_message.h"

void print_buffer(mc_buffer_t* buffer);
void print_option(mc_option_t* option);
void print_options(mc_options_list_t* list);
void print_token(mc_buffer_t* token);
void print_msg(mc_message_t* const msg);

#endif
