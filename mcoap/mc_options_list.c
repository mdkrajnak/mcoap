/**
 * @file
 * @ingroup options_list
 * @{
 */

#include "msys/ms_memory.h"
#include "mcoap/mc_options_list.h"

mc_options_list_t* mc_options_list_alloc() {
    return (mc_options_list_t*)ms_calloc(1, sizeof(mc_options_list_t));
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

uint32_t mc_options_list_buffer_size(mc_options_list_t* list) {
	mc_option_t* current = list->options;
	uint32_t prev_option_num = 0;
	uint32_t size = 0;
	uint32_t noption;

	for (noption = 0; noption < list->noptions; noption++) {
		size += mc_option_buffer_size(current, prev_option_num);
		prev_option_num = current->option_num;
		current++;
	}
	return size;
}

/**
 * @return 0 if unable to convert
 */
static int int_to_extended(uint32_t value, uint8_t* head, uint16_t* extended) {
	int result = 1;

	if (value < 13) {
		*head = value;
	}
	else if (value < 269) {
		*head = 13;
		*extended = value - 13;
	}
	else if (value < 65805) {
		*head = 14;
		*extended = value - 269;
	}
	else {
		result = 0;
	}
	return result;
}

/**
 * @return 0 if unable to convert
 */
static int int_to_extended_to_int(uint8_t head, uint16_t extended, uint32_t* value) {
	int result = 1;

	if (head < 13) {
		*value = head;
	}
	else if (head == 13) {
		*value = extended + 13;
	}
	else if (head == 14) {
		*value = extended + 269;
	}
	else {
		*value = 0;
		result = 0;
	}
	return result;
}

mc_buffer_t* mc_options_list_to_buffer(mc_options_list_t* list, mc_buffer_t* buffer) {
	return buffer;
}

mc_buffer_t* mc_options_list_mk_buffer(mc_options_list_t* list) {
	uint32_t count = mc_options_list_buffer_size(list);
	uint8_t* bytes = ms_calloc(count, sizeof(uint8_t));
	mc_buffer_t* buffer = mc_buffer_init(mc_buffer_alloc(), count, bytes);

	mc_options_list_to_buffer(list, buffer);

	return buffer;
}

/** @} */
