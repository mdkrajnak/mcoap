/**
 * @file
 * @ingroup message
 * @{
 */

#include <string.h>

#include "msys/ms_memory.h"
#include "msys/ms_endian.h"
#include "mcoap/mc_message.h"
#include "mcoap/mc_header.h"
#include "mcoap/mc_buffer.h"

#define MESSAGE_VERSION 1
#define MESSAGE_VERSION_MASK   0xC0000000
#define MESSAGE_TYPE_MASK      0x30000000
#define MESSAGE_TOKEN_LEN_MASK 0x0F000000
#define MESSAGE_CODE_MASK      0x00FF0000
#define MESSAGE_MSG_ID_MASK    0x0000FFFF

#define CONFIRMABLE     0
#define NONCONFIRMABLE  1
#define ACKNOWLEDGEMENT 2
#define RESET           3


mc_message_t* mc_message_alloc() {
    return ms_calloc(1, mc_message_t);
}

mc_message_t* mc_message_init(
    mc_message_t* message,
    uint8_t version,
    uint8_t message_type,
    uint8_t code,
    uint16_t message_id,
    mc_buffer_t token,
    mc_options_list_t options,
    mc_buffer_t payload) {

	message->header = mc_header_create(version, message_type, token.nbytes, code, message_id);
	message->token = token;
	message->options = options;
	message->payload = payload;

    return message;
}
    
mc_message_t* mc_message_con_init(
    mc_message_t* message,
    uint8_t code,
    uint16_t message_id,
    mc_buffer_t token,
    mc_options_list_t options,
    mc_buffer_t payload) {
    
    return mc_message_init(message, MESSAGE_VERSION, CONFIRMABLE, code, message_id, token, options, payload);
}

mc_message_t* mc_message_non_init(
    mc_message_t* message,
    uint8_t code,
    uint16_t message_id,
    mc_buffer_t token,
    mc_options_list_t options,
    mc_buffer_t payload) {
    
	return mc_message_init(message, MESSAGE_VERSION, NONCONFIRMABLE, code, message_id, token, options, payload);
}

mc_message_t* mc_message_ack_init(
    mc_message_t* message,
    uint8_t code,
    uint16_t message_id,
    mc_buffer_t token,
    mc_options_list_t options,
    mc_buffer_t payload) {
    
	return mc_message_init(message, MESSAGE_VERSION, ACKNOWLEDGEMENT, code, message_id, token, options, payload);
}
    
mc_message_t* mc_message_rst_init(
    mc_message_t* message,
    uint8_t code,
    uint16_t message_id,
    mc_buffer_t token,
    mc_options_list_t options,
    mc_buffer_t payload) {
    
	return mc_message_init(message, MESSAGE_VERSION, RESET, code, message_id, token, options, payload);
}
    
mc_message_t* mc_message_deinit(mc_message_t* message) {
    return 0;
}

uint8_t mc_message_get_version(mc_message_t* message) {
    return mc_header_get_version(message->header);
}

uint8_t mc_message_get_type(mc_message_t* message) {
    return mc_header_get_message_type(message->header);
}

uint8_t mc_message_get_token_len(mc_message_t* message) {
    return mc_header_get_token_length(message->header);
}

uint8_t mc_message_get_code(mc_message_t* message) {
    return mc_header_get_code(message->header);
}

uint16_t mc_message_get_message_id(mc_message_t* message) {
    return mc_header_get_message_id(message->header);
}

uint32_t mc_message_buffer_size(mc_message_t* message) {
	uint32_t size;

	if (message == 0) return 0;

	size = sizeof(message->header);
	size += message->token.nbytes;
	size += mc_options_list_buffer_size(&message->options);
	size += message->payload.nbytes;

    return size;
}

mc_buffer_t* mc_message_to_buffer(mc_message_t* message, mc_buffer_t* buffer) {
	uint32_t bpos = 0;
	uint32_t tmp = ms_swap_u64(message->header);

	if (mc_message_buffer_size(message) > buffer->nbytes) return 0;

	memcpy(buffer->bytes, &tmp, sizeof(uint32_t));
	bpos += sizeof(message->header);

	if (mc_buffer_copy_to(buffer, &bpos, &message->token) == 0) return 0;

	if (mc_options_list_to_buffer(&message->options, buffer, &bpos) == 0) return 0;

	if (message->payload.nbytes > 0) {
		buffer->bytes[bpos] = 0xff;
		bpos++;
	}

	if (mc_buffer_copy_to(buffer, &bpos, &message->payload) == 0) return 0;

    return buffer;
}

mc_buffer_t* mc_message_mk_buffer(mc_message_t* message) {
	uint32_t count;

	if (message == 0) return 0;

	count = mc_message_buffer_size(message);
	return mc_buffer_init(mc_buffer_alloc(), count, ms_calloc(count, uint8_t));
}

/** @} */
