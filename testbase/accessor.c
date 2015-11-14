#include "testbase/accessor.h"
#include "msys/ms_endian.h"
#include "mcoap/mc_header.h"

/**
 * Copy a token from a serialized buffer in the output confirmation queue.
 *
 * Intended for testing to extract the assigned token so we can compare it
 * to the token returned in a response.
 *
 * @return a new copy of the token.
 */
mc_buffer_t* mc_endpt_udp_copy_queued_token(mc_endpt_udp_t* endpt, uint16_t msgid) {
    mc_buffer_queue_t* queue = &endpt->confirmq;
    mc_buffer_queue_entry_t* entry = mc_buffer_queue_get(queue, msgid);
    uint32_t header;
    uint32_t tklen;
    uint32_t bpos;

    if (entry == 0) return 0;

    bpos = 0;
    header = ms_swap_u32(mc_buffer_next_uint32(entry->msg, &bpos));
    tklen = (uint32_t)mc_header_get_token_length(header);

    return mc_buffer_copy(entry->msg, 4, tklen);
}