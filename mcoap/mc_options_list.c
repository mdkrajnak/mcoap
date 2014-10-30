/**
 * @file
 * @ingroup options_list
 * @{
 */

#include <string.h>
#include "msys/ms_memory.h"
#include "msys/ms_endian.h"
#include "mcoap/mc_options_list.h"

mc_options_list_t* mc_options_list_alloc() {
    return ms_calloc(1, mc_options_list_t);
}

mc_options_list_t* mc_options_list_deinit(mc_options_list_t* list) {

	mc_option_ndeinit(list->options, list->noptions);
	ms_free(list->options);

	list->noptions = 0;
	list->options = 0;

    return list;
}

/*
 * Compare two items by tag.  Used to sort a list of items by tag.
 * Return a negative number if less then, a positive number if greater then, or 0 if equal.
 */
static int compare_options (const void * left, const void * right) {
	mc_option_t* ileft = (mc_option_t*)left;
	mc_option_t* iright = (mc_option_t*)right;

    return ( (ileft->option_num) - (iright->option_num) );
}

/** Sort an array of items by tag. */
static void options_sort(uint32_t count, mc_option_t* items) {
    qsort(items, count, sizeof(mc_options_list_t), compare_options);
}

mc_options_list_t* mc_options_list_init(mc_options_list_t* list, uint32_t noptions, mc_option_t* options) {
    list->noptions = noptions;
    list->options = options;
    if (options) options_sort(list->noptions, list->options);

	return list;
}

uint32_t mc_options_list_buffer_size(const mc_options_list_t* list) {
	mc_option_t* current = list->options;
	uint32_t prev_option_num = 0;
	uint32_t size = 0;
	uint32_t noption;

	for (noption = 0; noption < list->noptions; noption++) {
		size += mc_option_buffer_size(current, prev_option_num);
		prev_option_num = current->option_num;
		current++;
	}

	/* Increment by 1 for the 0xff at the end of the buffer list. */
	size++;
	return size;
}

/**
 * @return 1: if 4 bits, 2: +8 bits, 3: +16 bits. 0 if unable to convert
 */
static int value_to_encoded_int(uint32_t value, uint8_t* head, uint16_t* extended) {
	int result;

	if (value < 13) {
		*head = value;
		result = 1;
	}
	else if (value < 269) {
		*head = 13;
		*extended = value - 13;
		result = 2;
	}
	else if (value < 65805) {
		*head = 14;
		*extended = value - 269;
		result = 3;
	}
	else {
		result = 0;
	}
	return result;
}

/**
 * @param head the 4 bit int "header"
 * @param extended the 1-2 byte extended integer information.
 * @param value the return value.
 * @return 1: if 4 bits, 2: 8 bits, 3: 16 bits. 0 if unable to convert
 */
static int encoded_int_to_value(uint8_t head, uint16_t extended, uint16_t* value) {
	int result = 0;

	if (head < 13) {
		*value = head;
		result = 1;
	}
	else if (head == 13) {
		*value = extended + 13;
		result = 2;
	}
	else if (head == 14) {
		*value = extended + 269;
		result = 3;
	}
	else {
		*value = 0;
		result = 0;
	}
	return result;
}

static mc_buffer_t* option_to_buffer(mc_buffer_t* buffer, const mc_option_t* option, uint32_t prev_option_num, uint32_t* bpos) {
	uint8_t num_head = 0;
	uint16_t num_ext = 0;
	uint8_t len_head = 0;
	uint16_t len_ext = 0;
	uint8_t delta = option->option_num - prev_option_num;

	/** Check that there is at least 1 byte in the buffer. */
	if ((buffer->nbytes - *bpos) < 1) return 0;

	uint32_t ncode = value_to_encoded_int(delta, &num_head, &num_ext);
	if (ncode == 0) return 0;

	uint32_t lcode = value_to_encoded_int(option->value.nbytes, &len_head, &len_ext);
	if (lcode == 0) return 0;

	uint8_t byte = (num_head << 4) | len_head;
	buffer->bytes[*bpos] = byte;
	(*bpos)++;

	if (ncode == 2) {
		if ((buffer->nbytes - *bpos) < 1) return 0;
		buffer->bytes[*bpos] = (uint8_t)num_ext;
		(*bpos)++;
	}
	else if (ncode == 3) {
		if ((buffer->nbytes - *bpos) < 2) return 0;
		uint16_t tmp = ms_swap_u16(num_ext);
		memcpy(&(buffer->bytes[*bpos]), &tmp, 2);
		(*bpos) += 2;
	}

	if (lcode == 2) {
		if ((buffer->nbytes - *bpos) < 1) return 0;
		buffer->bytes[*bpos] = (uint8_t)len_ext;
		(*bpos)++;
	}
	else if (lcode == 3) {
		if ((buffer->nbytes - *bpos) < 2) return 0;
		uint16_t tmp = ms_swap_u16(len_ext);
		memcpy(&(buffer->bytes[*bpos]), &tmp, 2);
		(*bpos) += 2;
	}

	if ((buffer->nbytes - *bpos) < option->value.nbytes) return 0;
	memcpy(&(buffer->bytes[*bpos]), option->value.bytes, option->value.nbytes);
	(*bpos) += option->value.nbytes;

	return buffer;
}

/**
 * Design Note!
 *
 * In the case of a failure we want to know the buffer position where the problem occurred.
 * This means we need a different field to indicate failure.
 * To that end we return a pointer to the buffer with 0 for failure and pass the
 * position in by reference.
 */

/** @return pointer to modified buffer or 0 if failure. */
mc_buffer_t* mc_options_list_to_buffer(const mc_options_list_t* list, mc_buffer_t* buffer, uint32_t* bpos) {
	uint32_t ioption;
	uint32_t prev_option_num = 0;
	uint32_t apos = 0;

	/* If not caller specified, use position 0. */
	if (bpos == 0) bpos = &apos;

	for (ioption = 0; ioption < list->noptions; ioption++) {
		buffer = option_to_buffer(buffer, &(list->options[ioption]), prev_option_num, bpos);

		/* Check for error. */
		if (buffer == 0) return 0;

		prev_option_num = list->options[ioption].option_num;
	}

	/* Put the end of option list marker on. */
	buffer->bytes[*bpos] = 0xff;
	(*bpos)++;
	buffer->bytes[*bpos] = 0xff;

	return buffer;
}

mc_buffer_t* mc_options_list_mk_buffer(const mc_options_list_t* list) {
	uint32_t bpos = 0;
	uint32_t count = mc_options_list_buffer_size(list);
	uint8_t* bytes = ms_calloc(count, uint8_t);
	mc_buffer_t* buffer = mc_buffer_init(mc_buffer_alloc(), count, bytes);

	mc_options_list_to_buffer(list, buffer, &bpos);

	return buffer;
}

static uint32_t count_options(const mc_buffer_t* buffer) {
	uint8_t byte;
	uint8_t delta;
	uint8_t len;
	uint16_t extended;
	uint16_t value;
	uint32_t used = 0;
	uint32_t total = 0;

	/* Minimum buffer size is 3, one byte for the lead dela/len header, */
	/* one byte for the data, and one byte for the terminating 0xff. */
	if (buffer->nbytes < 4) return 0;
	byte = buffer->bytes[used];
	used++;

	while (byte != 0xff) {
		delta = byte >> 4;
		len = byte & 0x0f;

		/* Skip the extended part of the option delta. */
		if (delta == 13) {
			used += 1;
		}
		else if (delta == 14) {
			used+=2;
		}

		/* Extract the option length and skip over it. */
		if (len == 13) {
			extended = buffer->bytes[used];
			used++;
			encoded_int_to_value(len, extended, &value);
			used += value;
		}
		else if (len == 14) {
			memcpy(&extended, buffer->bytes + used, 2);
			used += 2;
			extended = ms_swap_u16(extended);
			encoded_int_to_value(len, extended, &value);
			used += value;
		}

		/* Check to see if implied size overflows the remaining bytes in the buffer. */
		if (used > buffer->nbytes) return 0;

		byte = buffer->bytes[used];
		used++;
	}

	return total;
}

/** How to signal an error? */
static uint16_t option_num_from_buffer(const mc_buffer_t* buffer, uint8_t prefix, uint32_t* bpos) {
	uint16_t extended;
	uint16_t value;

	/* Skip the extended part of the option delta. */
	if (prefix == 13) {
		extended = mc_buffer_next_uint8(buffer, bpos);
		encoded_int_to_value(prefix, extended, &value);
	}
	else if (prefix == 14) {
		extended = mc_buffer_next_uint16(buffer, bpos);
		encoded_int_to_value(prefix, extended, &value);
	}
	else {
		value = prefix;
	}

	return value;
}

static mc_option_t* option_from_buffer(mc_option_t* option, const mc_buffer_t* buffer, uint16_t prevnum, uint32_t* bpos) {
	uint16_t optnum;
	uint16_t optlen;
	uint8_t* optbytes;
	uint8_t byte;
	uint8_t delta;
	uint8_t len;

	/* Minimum buffer size is 3, one byte for the lead dela/len header, */
	/* one byte for the data, and one byte for the terminating 0xff. */
	if (buffer->nbytes < 4) return 0;

	byte = mc_buffer_next_uint8(buffer, bpos);

	delta = byte >> 4;
	len = byte & 0x0f;

	optnum = option_num_from_buffer(buffer, delta, bpos);
	optnum += prevnum;

	optlen = option_num_from_buffer(buffer, len, bpos);
	optbytes = mc_buffer_next_ptr(buffer, optlen, bpos);

	return mc_option_init(option, optnum, optlen, optbytes);
}

/** @return pointer to created list buffer or 0 if failure. */
mc_options_list_t* mc_buffer_to_option_list(const mc_buffer_t* buffer, uint32_t* bpos) {
	mc_option_t* options;
	mc_option_t* current;
	uint16_t prevnum;
	uint32_t ioption;
	uint32_t apos = 0;
	uint32_t noptions = count_options(buffer);

	if (noptions == 0) return 0;
	options = mc_option_nalloc(noptions);

	/* Use position 0 if buffer position not specified. */
	if (bpos == 0) bpos = &apos;

	current = options;
	current = option_from_buffer(current, buffer, 0, bpos);
	if (current == 0) return 0;

	for (ioption = 1; ioption < noptions; ioption++) {
		prevnum = current->option_num;
		current++;
		current = option_from_buffer(current, buffer, prevnum, bpos);
		if (current == 0) return 0;
	}

	return mc_options_list_init(mc_options_list_alloc(), noptions, options);
}

/** @} */
