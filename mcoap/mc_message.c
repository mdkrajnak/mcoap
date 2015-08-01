/**
 * @file
 * @ingroup message
 * @{
 */

#include <string.h>

#include "msys/ms_memory.h"
#include "msys/ms_copy.h"
#include "msys/ms_endian.h"
#include "msys/ms_log.h"
#include "mcoap/mc_options_list.h"
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
    mc_buffer_t* token,
    mc_options_list_t* options,
    mc_buffer_t* payload) {

	message->header = mc_header_create(version, message_type, token->nbytes, code, message_id);
	message->token = token;
	message->options = options;
	message->payload = payload;

    return message;
}
    
mc_message_t* mc_message_con_init(
    mc_message_t* message,
    uint8_t code,
    uint16_t message_id,
    mc_buffer_t* token,
    mc_options_list_t* options,
    mc_buffer_t* payload) {
    
    return mc_message_init(message, MESSAGE_VERSION, CONFIRMABLE, code, message_id, token, options, payload);
}

mc_message_t* mc_message_non_init(
    mc_message_t* message,
    uint8_t code,
    uint16_t message_id,
    mc_buffer_t* token,
    mc_options_list_t* options,
    mc_buffer_t* payload) {
    
	return mc_message_init(message, MESSAGE_VERSION, NONCONFIRMABLE, code, message_id, token, options, payload);
}

mc_message_t* mc_message_ack_init(
    mc_message_t* message,
    uint8_t code,
    uint16_t message_id,
    mc_buffer_t* token,
    mc_options_list_t* options,
    mc_buffer_t* payload) {
    
	return mc_message_init(message, MESSAGE_VERSION, ACKNOWLEDGEMENT, code, message_id, token, options, payload);
}
    
mc_message_t* mc_message_rst_init(
    mc_message_t* message,
    uint8_t code,
    uint16_t message_id,
    mc_buffer_t* token,
    mc_options_list_t* options,
    mc_buffer_t* payload) {
    
	return mc_message_init(message, MESSAGE_VERSION, RESET, code, message_id, token, options, payload);
}
    
mc_message_t* mc_message_deinit(mc_message_t* message) {
	if (message->token) {
		ms_free(mc_buffer_deinit(message->token));
		message->token = 0;
	}
	if (message->options) {
		ms_free(mc_options_list_deinit(message->options));
		message->options = 0;
	}
	if (message->payload) {
		ms_free(mc_buffer_deinit(message->payload));
		message->payload = 0;
	}

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
	size += message->token->nbytes;
	size += mc_options_list_buffer_size(message->options);

	// If there are payload bytes and 1 for the payload 0xff flag
	// in addition to the payload bytes.
	if (message->payload && message->payload->nbytes > 0) {
		size++;
		size += message->payload->nbytes;
	}

    return size;
}

uint32_t mc_message_to_buffer(mc_message_t* message, mc_buffer_t* buffer) {
	uint32_t bpos = 0;
	uint32_t tmp = ms_swap_u32(message->header);

	if (mc_message_buffer_size(message) > buffer->nbytes) {
		ms_log_debug("buffer is too small small for message, %d < %d", buffer->nbytes, mc_message_buffer_size(message));
		return 0;
	}

	memcpy(buffer->bytes, &tmp, sizeof(uint32_t));
	bpos += sizeof(message->header);

	if (mc_buffer_copy_to(buffer, bpos, message->token, 0, message->token->nbytes) == 0) return 0;
	bpos += message->token->nbytes;

	if (mc_options_list_to_buffer(message->options, buffer, &bpos) == 0) return 0;

	/* If there is a payload, append the start of payload marker and the payload. */
	if (message->payload && message->payload->nbytes > 0) {
		buffer->bytes[bpos] = 0xff;
		bpos++;

		if (mc_buffer_copy_to(buffer, bpos, message->payload, 0, message->payload->nbytes) == 0) return 0;
		bpos += message->payload->nbytes;
	}


    return bpos;
}

mc_message_t* mc_message_from_buffer(mc_message_t* message, mc_buffer_t* buffer, uint32_t* bpos) {
	uint32_t remaining;
	uint32_t marker;
	uint32_t pllen;
	uint32_t tklen;
	uint8_t* tkdata = 0;
	uint32_t apos = 0;

	if (message == 0) return 0;
	if (buffer == 0) return 0;

	/* Must have enough bytes for the header. */
	if (buffer->nbytes < 4) {
		ms_log_debug("buffer is less than the minimum size (4): %d", buffer->nbytes);
		return 0;
	}

	/* If no bpos pointer passed in, start at beginning of buffer. */
	if (bpos == 0) bpos = &apos;

	/* Read message components. */
	message->header = ms_swap_u32(mc_buffer_next_uint32(buffer, bpos));
	tklen = mc_header_get_token_length(message->header);
	tkdata = mc_buffer_next_ptr(buffer, tklen, bpos);

	/* N.B. Assumes token and options are null. */
	message->token = mc_buffer_init(mc_buffer_alloc(), tklen, ms_copy_uint8(tklen, tkdata));
	message->options = mc_options_list_from_buffer(mc_options_list_alloc(), buffer, bpos);

	remaining = buffer->nbytes - *bpos;

	if (remaining > 1) {
		/* Advance past beginning of payload marker. */
		marker = buffer->bytes[*bpos];
		if (marker != 0xff) {
			ms_log_debug("Invalid beginning of payload marker: 0x%x", marker);
			return 0;
		}
		(*bpos)++;

		pllen = remaining - 1;
		message->payload = mc_buffer_init(mc_buffer_alloc(), pllen, ms_calloc(pllen, uint8_t));

		if (mc_buffer_copy_to(message->payload, 0, buffer, *bpos, pllen) == 0) return 0;
		*bpos += pllen;
	}

	/* Todo, should we check the header version? */

	return message;
}

/** @} */
