#ifndef MC_MESSAGE_H
#define MC_MESSAGE_H

/** 
 * @file
 * @defgroup message CoAP Message
 * @{
 */

#include "mcoap/mc_options_list.h"


typedef struct mc_message mc_message_t;
struct mc_message {
    uint32_t header;
    mc_buffer_t token;
    mc_options_list_t options;
    mc_buffer_t payload;
};

mc_message_t* mc_message_alloc();
mc_message_t* mc_message_deinit(mc_message_t* message);

mc_message_t* mc_message_init(
    mc_message_t* message,
    uint8_t version,
    uint8_t message_type,
    uint8_t code,
    uint16_t message_id,
    mc_buffer_t token,
    mc_options_list_t options,
    mc_buffer_t payload);
    
mc_message_t* mc_message_con_init(
    mc_message_t* message,
    uint8_t code,
    uint16_t message_id,
    mc_buffer_t token,
    mc_options_list_t options,
    mc_buffer_t payload); 

mc_message_t* mc_message_non_init(
    mc_message_t* message,
    uint8_t code,
    uint16_t message_id,
    mc_buffer_t token,
    mc_options_list_t options,
    mc_buffer_t payload); 

mc_message_t* mc_message_ack_init(
    mc_message_t* message,
    uint8_t code,
    uint16_t message_id,
    mc_buffer_t token,
    mc_options_list_t options,
    mc_buffer_t payload);
    
mc_message_t* mc_message_rst_init(
    mc_message_t* message,
    uint8_t code,
    uint16_t message_id,
    mc_buffer_t token,
    mc_options_list_t options,
    mc_buffer_t payload); 
    
uint8_t mc_message_get_version(mc_message_t* message);
uint8_t mc_message_get_type(mc_message_t* message);
uint8_t mc_message_get_token_len(mc_message_t* message);
uint8_t mc_message_get_code(mc_message_t* message);
uint16_t mc_message_get_message_id(mc_message_t* message);

uint32_t mc_message_buffer_size(mc_message_t* message);
mc_buffer_t* mc_message_to_buffer(mc_message_t* message, mc_buffer_t* buffer);
mc_message_t* mc_message_from_buffer(mc_message_t* message, mc_buffer_t* buffer, uint32_t* bpos);


/** @} */

#endif

