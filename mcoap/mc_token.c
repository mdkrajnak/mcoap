/**
 * @file
 * @ingroup mc_token.c
 * @{
 */

#include <stdlib.h>
#include <string.h>

#include "msys/ms_memory.h"
#include "mcoap/mc_token.h"

/**
 * Create a token with caller specified 1 byte prefix and 4 byte
 * random suffix. The caller prefix can be an incrementing number
 * that helps minimize the chance of collisions.
 */
mc_buffer_t* mc_token_create1(uint8_t prefix) {
	uint8_t* buffer = ms_calloc(5, uint8_t);

	uint32_t suffix = rand();
	size_t len = sizeof(int);

	if (len > 4) len = 4;

	buffer[0] = prefix;
	memcpy(&buffer[1], &suffix, sizeof(uint32_t));

	return mc_buffer_init(mc_buffer_alloc(), 5, buffer);
}

/**
 * Create a token with caller specified 2 byte prefix and 4 byte
 * random suffix. The caller prefix can be an incrementing number
 * that helps minimize the chance of collisions.
 */
mc_buffer_t* mc_token_create2(uint16_t prefix) {
	uint8_t* buffer = ms_malloc(6, uint8_t);

	uint32_t suffix = rand();
	size_t len = sizeof(int);

	if (len > 4) len = 4;

	memcpy(&buffer[0], &prefix, sizeof(uint16_t));
	memcpy(&buffer[2], &suffix, sizeof(uint32_t));

	return mc_buffer_init(mc_buffer_alloc(), 6, buffer);
}

/**
 * Create a token with caller specified 4 byte prefix and 4 byte
 * random suffix. The caller prefix can be an incrementing number
 * that helps minimize the chance of collisions.
 */
mc_buffer_t* mc_token_create4(uint32_t prefix) {
	uint8_t* buffer = ms_malloc(8, uint8_t);

	uint32_t suffix = rand();
	size_t len = sizeof(int);

	if (len > 4) len = 4;

	memcpy(&buffer[0], &prefix, sizeof(uint32_t));
	memcpy(&buffer[4], &suffix, sizeof(uint32_t));

	return mc_buffer_init(mc_buffer_alloc(), 8, buffer);
}

/** @} */
