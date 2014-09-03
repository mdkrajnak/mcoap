/**
 * @file
 * @ingroup buffer
 * @{
 */

#include "msys/ms_memory.h"
#include "mcoap/mc_buffer.h"

/**
 * Allocate a mc_buffer_t struct.
 */
mc_buffer_t* mc_buffer_alloc() {
    return (mc_buffer_t*)ms_calloc(1, sizeof(mc_buffer_t));
}

/**
 * Initialize a mc_buffer_t struct.
 */
mc_buffer_t* mc_buffer_init(mc_buffer_t* buffer, uint32_t nbytes, uint8_t* bytes) {
    buffer->nbytes = nbytes;
    if (buffer->bytes) ms_free(buffer->bytes);
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

/** @} */
