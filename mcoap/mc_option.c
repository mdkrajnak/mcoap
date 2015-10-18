
/**
 * @file
 * @ingroup option
 * @{
 */

#include "msys/ms_endian.h"
#include "msys/ms_copy.h"
#include "msys/ms_memory.h"
#include "mcoap/mc_option.h"

#include <string.h>

/**
 * Allocate a mc_option_t struct.
 */
mc_option_t* mc_option_alloc() {
    return ms_calloc(1, mc_option_t);
}

/**
 * Allocate a block of mc_option_t structs.
 */
mc_option_t* mc_option_nalloc(uint32_t count) {
    return ms_calloc(count, mc_option_t);
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
mc_option_t* mc_option_ndeinit(mc_option_t* options, uint32_t count) {
	mc_option_t* current = options;
	uint32_t noption;

	for (noption = 0; noption < count; noption++) {
		mc_option_deinit(current);
		current++;
	}

    return options;
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
 * Initialize a mc_option_t struct with a C string.
 * Note that the trailing null is not included.
 */
mc_option_t* mc_option_init_str(mc_option_t* option, uint16_t option_num, char* value) {
    option->option_num = option_num;
    mc_buffer_init(&option->value, strlen(value), (uint8_t*)value);

    return option;
}

/**
 * Initialize a mc_option_t struct with a uint32.
 * Note the value is stored in "in memory" format, no compression, no swapping.
 */
mc_option_t* mc_option_init_uint32(mc_option_t* option, uint16_t option_num, uint32_t value) {
    option->option_num = option_num;
    if (option->value.bytes) ms_free(option->value.bytes);
    if (value <= UINT8_MAX) {
    	uint8_t temp = value;
    	mc_buffer_init(&option->value, 1, ms_copy_uint8(1, &temp));
    }
    else if (value <= UINT16_MAX) {
    	uint16_t temp = value;
    	temp = ms_swap_u16(temp);
    	mc_buffer_init(&option->value, 2, (uint8_t*)ms_copy_uint16(1, &temp));
    }
    else {
    	uint32_t temp = ms_swap_u32(value);
    	mc_buffer_init(&option->value, 4, (uint8_t*)ms_copy_uint32(1, &temp));
    }

    return option;
}

uint32_t mc_option_as_uint32(const mc_option_t* option) {
	uint32_t result;

	if (option == 0) return 0;
	if (option->value.bytes == 0) return 0;

	if (option->value.nbytes == 1) {
		result = *option->value.bytes;
	}
	else if (option->value.nbytes == 2) {
		result = ms_swap_u16(*((uint16_t*)option->value.bytes));
	}
	else {
		result = ms_swap_u32(*((uint32_t*)option->value.bytes));
	}
    return result;
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
		result = 0;
	}
	return result;
}

/**
 * Compute the number of bytes to store the option in a network buffer.
 * Note it depends on the value of the previous option number when serializing
 * a list of options.
 */
uint32_t mc_option_buffer_size(const mc_option_t* option, uint32_t prev_option_num) {
	uint32_t delta = option->option_num - prev_option_num;
	uint32_t nbytes = option->value.nbytes;

	return extended_int_size(delta) + extended_int_size(nbytes) + nbytes - 1;
}

/** @} */
