
/**
 * @file
 * @ingroup option
 * @{
 */

#include "msys/ms_memory.h"
#include "mcoap/mc_option.h"

/**
 * Allocate a mc_option_t struct.
 */
mc_option_t* mc_option_alloc() {
    return (mc_option_t*)ms_calloc(1, sizeof(mc_option_t));
}

/**
 * Allocate a block of mc_option_t structs.
 */
mc_option_t* mc_option_nalloc(uint32_t count) {
    return (mc_option_t*)ms_calloc(count, sizeof(mc_option_t));
}

/**
 * Free the contents of a mc_option_t struct.
 */
mc_option_t* mc_option_deinit(mc_option_t* option) {
    mc_buffer_deinit(&option->value);

    return option;
}

/**
 * Free the contents of a block of mc_option_t structs.
 */
mc_option_t* mc_option_ndeinit(mc_option_t* option, uint32_t count) {
	mc_option_t* current = option;
	uint32_t noption;

	for (noption = 0; noption < count; noption++) {
		mc_option_deinit(current);
		current++;
	}

    return option;
}

/**
 * Initialize a mc_option_t struct.
 */
mc_option_t* mc_option_init(mc_option_t* option, uint16_t option_num, uint32_t nbytes, uint8_t* bytes) {
    option->option_num = option_num;
    option->value.nbytes = nbytes;
    if (option->value.bytes) ms_free(option->value.bytes);
    option->value.bytes = bytes;

    return option;
}

/**
 * Initialize a mc_option_t struct with a uint32.
 */
mc_option_t* mc_option_init_uint32(mc_option_t* option, uint16_t option_num, uint32_t value) {
    return 0;
}

uint32_t mc_option_as_uint32(mc_option_t* option) {
    return 0;
}

static uint32_t extended_int_size(uint32_t delta) {
	uint32_t result;

	if (delta < 13) {
		result = 1;
	}
	else if (delta < 269) {
		result = 2;
	}
	else if (delta < 65805) {
		result = 3;
	}
	else {
		result = 0
	}
	return result;
}

/**
 * Compute the number of bytes to store the option in a network buffer.
 * Note it depends on the value of the previous option number when serializing
 * a list of options.
 */
uint32_t mc_option_buffer_size(mc_option_t* option, uint32_t prev_option_num) {
	uint32_t delta = option->option_num - prev_option_num;
	uint32_t nbytes = option->value->nbytes;

	return extended_int_size(delta) + extended_int_size(nbytes) + nbytes - 1;
}

/** @} */
