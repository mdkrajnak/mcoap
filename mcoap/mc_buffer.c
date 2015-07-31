/**
 * @file
 * @ingroup buffer
 * @{
 */

#include <string.h>
#include "msys/ms_memory.h"
#include "mcoap/mc_buffer.h"

/**
 * Allocate a mc_buffer_t struct.
 */
mc_buffer_t* mc_buffer_alloc() {
    return ms_calloc(1, mc_buffer_t);
}

/**
 * Initialize a mc_buffer_t struct.
 */
mc_buffer_t* mc_buffer_init(mc_buffer_t* buffer, uint32_t nbytes, uint8_t* bytes) {
    buffer->nbytes = nbytes;
    buffer->bytes = bytes;

    return buffer;
}

/**
 * Free the contents of a mc_buffer_t struct.
 */
mc_buffer_t* mc_buffer_deinit(mc_buffer_t* buffer) {
    if (buffer->bytes) ms_free(buffer->bytes);
    buffer->nbytes = 0;
    buffer->bytes = 0;

    return buffer;
}

uint8_t mc_buffer_next_uint8(const mc_buffer_t* buffer, uint32_t* bpos) {
	uint8_t byte = buffer->bytes[*bpos];
	(*bpos)++;

	return byte;
}

/* Note, no swapping. */
uint16_t mc_buffer_next_uint16(const mc_buffer_t* buffer, uint32_t* bpos) {
	uint16_t result;

	memcpy(&(buffer->bytes[*bpos]), &result, 2);
	(*bpos) += 2;

	return result;
}

/* Note, no swapping. */
uint32_t mc_buffer_next_uint32(const mc_buffer_t* buffer, uint32_t* bpos) {
	uint32_t result;

	memcpy(&result, &(buffer->bytes[*bpos]), 4);
	(*bpos) += 4;

	return result;
}

/* Note, no swapping. */
uint8_t* mc_buffer_next_ptr(const mc_buffer_t* buffer, uint32_t len, uint32_t* bpos) {
	uint8_t* ptr;


	/* Make sure there is enough bytes */
	if ((buffer->nbytes - len - *bpos) < 0) return 0;

	ptr = buffer->bytes + *bpos;
	(*bpos) += len;

	return ptr;
}

/** Copy without performing size checks. */
static mc_buffer_t* copy_to(mc_buffer_t* dest, uint32_t len, uint32_t* dpos, const mc_buffer_t* src, uint32_t* spos) {
	memcpy(&dest->bytes[*dpos], &src->bytes[*spos], len);
	*dpos += len;
	*spos += len;

	return dest;
}

mc_buffer_t* mc_buffer_copy_to(mc_buffer_t* dest, uint32_t len, uint32_t* dpos, const mc_buffer_t* src, uint32_t* spos) {
	uint32_t dzero = 0;
	uint32_t szero = 0;

	if (dest == 0) return dest;
	if (src == 0) return dest;

	/* Use zero as default dest/src position. */
	if (dpos == 0) dpos = &dzero;
	if (spos == 0) spos = &szero;

	/* Make sure there's enough room, if not copy nothing. */
	/* Caller must check bpos to see if anything is written. */
	if (dest->nbytes < *dpos) return dest;
	if ((dest->nbytes - *dpos) < len) return dest;

	if (src->nbytes < *spos) return dest;
	if ((src->nbytes - *spos) < len) return dest;

	return copy_to(dest, len, dpos, src, spos);
}

mc_buffer_t* mc_buffer_copy(const mc_buffer_t* src, uint32_t len, uint32_t* spos) {
	mc_buffer_t* result;
	uint32_t zero = 0;

	if (src == 0) return 0;
	if (spos == 0) spos = &zero;

	if (src->nbytes < *spos) return 0;
	if ((src->nbytes - *spos) < len) return 0;

	result = mc_buffer_init(mc_buffer_alloc(), len, ms_calloc(len, uint8_t));
	return copy_to(result, len, 0, src, spos);
}

/** @} */
