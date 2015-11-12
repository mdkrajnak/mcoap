#include <mcoap/mc_buffer.h>
#include "compare.h"

/** @return 1 if the buffers are equal, 0 otherwise. */
int mc_buffer_eq(mc_buffer_t* left, mc_buffer_t* right) {
    uint32_t ibyte;

    if (left == right) return 1;
    if (left == 0) return 0;
    if (right == 0) return 0;

    if (left->nbytes != right->nbytes) return 0;

    for (ibyte = 0; ibyte < left->nbytes; ibyte++) {
        if (left->bytes[ibyte] != right->bytes[ibyte]) return 0;
    }
    return 1;
}
