#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "msys/ms_copy.h"
#include "msys/ms_memory.h"
#include "mcoap/mc_options_list.h"
#include "testmc/mc_options_list_test.h"

#include "cutest/CuTest.h"

/**
 *  Given an option
 *  When we create an option list with that option,
 *  Then the list contains that option.
 */
static void test_options_list_init(CuTest* tc) {
	uint16_t one16 = 1;
	mc_buffer_t* buffer = mc_buffer_init(mc_buffer_alloc(), sizeof(uint16_t), (uint8_t*)ms_copy_uint16(1, &one16));
	mc_option_t* option = mc_option_init(mc_option_alloc(), 0, buffer->nbytes, buffer->bytes);
    mc_options_list_t* list = mc_options_list_init(mc_options_list_alloc(), 1, option);

    CuAssert(tc, "list is not null", list != 0);
    CuAssert(tc, "list count is 1", list->noptions == 1);
    CuAssert(tc, "list owns the data buffer", list->options->value.bytes == buffer->bytes);

    free(mc_options_list_deinit(list));
    free(buffer);
}

static FILE* write_bytes(FILE* out, uint32_t count, uint8_t* bytes) {
	uint32_t ibyte;

	fprintf(out, "bytes:");
	for(ibyte = 0; ibyte < count; ibyte++) {
		fprintf(out, " 0x%02x", bytes[ibyte]);
	}
	fprintf(out, "\n");

	return out;
}

/**
 *  Given an option list with two small (ints that fit into an uint8) options,
 *  When we serialize it to a buffer,
 *  Then the buffer contains the bytes we would expect for the wire format.
 */
static void test_options_list_u8_write_to_buffer(CuTest* tc) {
	uint32_t bpos = 0;
	mc_option_t* options = mc_option_nalloc(2);
	mc_option_init_uint32(options    , 1, 1);
	mc_option_init_uint32(options + 1, 2, 2);

    mc_options_list_t* list = mc_options_list_init(mc_options_list_alloc(), 2, options);

    uint32_t nbytes = mc_options_list_buffer_size(list);
    mc_buffer_t* buffer = mc_buffer_init(mc_buffer_alloc(), nbytes, ms_calloc(nbytes, uint8_t));

    CuAssert(tc, "size is 4", nbytes == 4);
    mc_options_list_to_buffer(list, buffer, &bpos);

    write_bytes(stdout, buffer->nbytes, buffer->bytes);

    /* First byte is delta of 1 in the first 4 bits, length of 1 is second 4 bits. */
    CuAssert(tc, "bytes[0] is 0x11", buffer->bytes[0] == 0x11);

    /* Value of 1 in 8 bits. */
    CuAssert(tc, "bytes[1] is 0x01", buffer->bytes[1] == 0x01);

    /* First byte is delta of 1 in the first 4 bits, length of 1 is second 4 bits. */
    CuAssert(tc, "bytes[2] is 0x11", buffer->bytes[2] == 0x11);

    /* Value of 1 in 8 bits. */
    CuAssert(tc, "bytes[3] is 0x02", buffer->bytes[3] == 0x02);

    free(mc_options_list_deinit(list));
    free(mc_buffer_deinit(buffer));
}

/**
 *  Given an option list with two uint16 sized options ,
 *  When we serialize it to a buffer,
 *  Then the buffer contains the bytes we would expect for the wire format.
 */
static void test_options_list_u16_write_to_buffer(CuTest* tc) {
	uint32_t bpos = 0;
	mc_option_t* options = mc_option_nalloc(2);
	mc_option_init_uint32(options    , 1, 256);
	mc_option_init_uint32(options + 1, 2, 257);

    mc_options_list_t* list = mc_options_list_init(mc_options_list_alloc(), 2, options);

    uint32_t nbytes = mc_options_list_buffer_size(list);
    mc_buffer_t* buffer = mc_buffer_init(mc_buffer_alloc(), nbytes, ms_calloc(nbytes, uint8_t));

    CuAssert(tc, "size is 6", nbytes == 6);
    mc_options_list_to_buffer(list, buffer, &bpos);

    write_bytes(stdout, buffer->nbytes, buffer->bytes);

    /* First byte is delta of 1 in the first 4 bits, length of 2 is second 4 bits. */
    CuAssert(tc, "bytes[0] is 0x12", buffer->bytes[0] == 0x12);

    /* Value is 256 in 16 bits in network byte order. */
    CuAssert(tc, "bytes[1] is 0x01", buffer->bytes[1] == 0x01);
    CuAssert(tc, "bytes[2] is 0x00", buffer->bytes[2] == 0x00);
    /* First byte is delta of 1 in the first 4 bits, length of 2 is second 4 bits. */
    CuAssert(tc, "bytes[3] is 0x12", buffer->bytes[3] == 0x12);

    /* Value is 257 in 16 bits in network byte order. */
    CuAssert(tc, "bytes[4] is 0x01", buffer->bytes[4] == 0x01);
    CuAssert(tc, "bytes[4] is 0x01", buffer->bytes[4] == 0x01);

    free(mc_options_list_deinit(list));
    free(mc_buffer_deinit(buffer));
}

/**
 *  Given an option list with two uint16 sized options ,
 *  When we serialize it to a buffer,
 *  Then the buffer contains the bytes we would expect for the wire format.
 */
static void test_options_list_u32_write_to_buffer(CuTest* tc) {
	uint32_t bpos = 0;
	mc_option_t* options = mc_option_nalloc(2);
	mc_option_init_uint32(options    , 1, 0x00010000);
	mc_option_init_uint32(options + 1, 2, 0x00010001);

    mc_options_list_t* list = mc_options_list_init(mc_options_list_alloc(), 2, options);

    uint32_t nbytes = mc_options_list_buffer_size(list);
    mc_buffer_t* buffer = mc_buffer_init(mc_buffer_alloc(), nbytes, ms_calloc(nbytes, uint8_t));

    CuAssert(tc, "size is 10", nbytes == 10);
    mc_options_list_to_buffer(list, buffer, &bpos);

    write_bytes(stdout, buffer->nbytes, buffer->bytes);

    /* First byte is delta of 1 in the first 4 bits, length of 2 is second 4 bits. */
    CuAssert(tc, "bytes[0] is 0x14", buffer->bytes[0] == 0x14);

    /* Value is 65536 in 32 bits in network byte order. */
    CuAssert(tc, "bytes[1] is 0x01", buffer->bytes[1] == 0x00);
    CuAssert(tc, "bytes[2] is 0x00", buffer->bytes[2] == 0x01);
    CuAssert(tc, "bytes[3] is 0x00", buffer->bytes[3] == 0x00);
    CuAssert(tc, "bytes[4] is 0x00", buffer->bytes[4] == 0x00);

    /* First byte is delta of 1 in the first 4 bits, length of 2 is second 4 bits. */
    CuAssert(tc, "bytes[5] is 0x14", buffer->bytes[5] == 0x14);

    /* Value is 65537 in 32 bits in network byte order. */
    CuAssert(tc, "bytes[6] is 0x01", buffer->bytes[6] == 0x00);
    CuAssert(tc, "bytes[7] is 0x01", buffer->bytes[7] == 0x01);
    CuAssert(tc, "bytes[8] is 0x01", buffer->bytes[8] == 0x00);
    CuAssert(tc, "bytes[9] is 0x01", buffer->bytes[9] == 0x01);

    free(mc_options_list_deinit(list));
    free(mc_buffer_deinit(buffer));
}

/**
 *  Given an option list with two uint16 sized options ,
 *  When we serialize it to a buffer,
 *  Then the buffer contains the bytes we would expect for the wire format.
 */
static void test_options_list_vinit(CuTest* tc) {
	mc_options_list_t* list = mc_options_list_vinit(
		mc_options_list_alloc(),
		3,
		mc_option_init_uint32(mc_option_alloc(), 1, 1),
		mc_option_init_uint32(mc_option_alloc(), 2, 256),
		mc_option_init_uint32(mc_option_alloc(), 3, 65536));

    CuAssert(tc, "size is 3", list->noptions == 3);

    CuAssert(tc, "option[0] num is 1", list->options[0].option_num == 1);
    CuAssert(tc, "option[0] val is 1", mc_option_as_uint32(&list->options[0]) == 1);

    CuAssert(tc, "option[1] num is 2", list->options[1].option_num == 2);
    CuAssert(tc, "option[1] val is 256", mc_option_as_uint32(&list->options[1]) == 256);

    CuAssert(tc, "option[2] num is 3", list->options[2].option_num == 3);
    CuAssert(tc, "option[2] val is 65536", mc_option_as_uint32(&list->options[2]) == 65536);

    free(mc_options_list_deinit(list));
}

/**
 *  Given an option list with two uint16 sized options ,
 *  When we serialize it to a buffer,
 *  Then the buffer contains the bytes we would expect for the wire format.
 */
static void test_options_list_buffer_roundtrip(CuTest* tc) {
// test_options_list_buffer_roundtrip(CuTest* tc) {
	uint32_t bpos = 0;
	uint8_t temp = 10;
	mc_option_t* options = mc_option_nalloc(5);
	mc_option_init_uint32(options    , 1, 0x00000001);
	mc_option_init_uint32(options + 1, 2, 0x00000100);
	mc_option_init_uint32(options + 2, 3, 0x00010000);
	mc_option_init_str(options + 3,    4, ms_copy_str("coap"));
	mc_option_init(options + 4,        5, 1, (uint8_t*)ms_copy_uint8(1, &temp));

    mc_options_list_t* list = mc_options_list_init(mc_options_list_alloc(), 5, options);

    uint32_t nbytes = mc_options_list_buffer_size(list);
    mc_buffer_t* buffer = mc_buffer_init(mc_buffer_alloc(), nbytes, ms_calloc(nbytes, uint8_t));

    CuAssert(tc, "size is 17", nbytes == 17);
    mc_options_list_to_buffer(list, buffer, &bpos);

    write_bytes(stdout, buffer->nbytes, buffer->bytes);

    bpos = 0;
    mc_options_list_t* result = mc_buffer_to_option_list(buffer, &bpos);

    CuAssert(tc, "result != 0", result != 0);
    CuAssert(tc, "There are 5 options", result->noptions == 5);

    CuAssert(tc, "result->options[0] optnum == 1", result->options[0].option_num == 1);
    CuAssert(tc, "result->options[0] optlen == 1", result->options[0].value.nbytes == 1);
    CuAssert(tc, "result->options[0] value == 0x00000001", mc_option_as_uint32(&result->options[0]) == 0x00000001);

    CuAssert(tc, "result->options[1] optnum == 2", result->options[1].option_num == 2);
    CuAssert(tc, "result->options[1] optlen == 2", result->options[1].value.nbytes == 2);
    CuAssert(tc, "result->options[1] value == 0x00000100", mc_option_as_uint32(&result->options[1]) == 0x00000100);

    CuAssert(tc, "result->options[2] optnum == 3", result->options[2].option_num == 3);
    CuAssert(tc, "result->options[2] optlen == 4", result->options[2].value.nbytes == 4);
    CuAssert(tc, "result->options[2] value == 0x00000100", mc_option_as_uint32(&result->options[2]) == 0x00010000);

    CuAssert(tc, "result->options[3] optnum == 4", result->options[3].option_num == 4);
    CuAssert(tc, "result->options[3] optlen == 4", result->options[3].value.nbytes == 4);
    CuAssert(tc, "result->options[3] value == 'coap'", strncmp((const char*)result->options[3].value.bytes, "coap", 4) == 0);

    CuAssert(tc, "result->options[4] optnum == 5", result->options[4].option_num == 5);
    CuAssert(tc, "result->options[4] optlen == 1", result->options[4].value.nbytes == 1);
    CuAssert(tc, "result->options[4] value == 10", *(result->options[4].value.bytes) == 10);

    free(mc_options_list_deinit(list));
    free(mc_buffer_deinit(buffer));
    free(mc_options_list_deinit(result));
}

/* Run all of the tests in this test suite. */
CuSuite* mc_options_list_suite() {
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, test_options_list_init);
    SUITE_ADD_TEST(suite, test_options_list_u8_write_to_buffer);
    SUITE_ADD_TEST(suite, test_options_list_u16_write_to_buffer);
    SUITE_ADD_TEST(suite, test_options_list_u32_write_to_buffer);
    SUITE_ADD_TEST(suite, test_options_list_vinit);
    SUITE_ADD_TEST(suite, test_options_list_buffer_roundtrip);

    return suite;
}
