#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "msys/ms_copy.h"
#include "msys/ms_memory.h"
#include "mcoap/mc_message.h"
#include "testmc/mc_message_test.h"

#include "cutest/CuTest.h"

/* @todo extract to common utilities. */
static FILE* write_bytes(FILE* out, const char* prefix, uint32_t count, uint8_t* bytes) {
	uint32_t ibyte;

	fprintf(out, "%s:", prefix);
	for(ibyte = 0; ibyte < count; ibyte++) {
		fprintf(out, " 0x%02x", bytes[ibyte]);
	}
	fprintf(out, "\n");

	return out;
}

/**
 *  Given an empty message,
 *  When we serialize it,
 *  Then we get the expected bytes.
 */
static void test_empty_message_to_buffer(CuTest* tc) {
	mc_buffer_t token;
	mc_options_list_t options;
	mc_buffer_t payload;
	uint8_t code = 0;
	uint16_t message_id = 0;

	mc_buffer_init(&token, 0, 0);
	mc_options_list_init(&options, 0, 0);
	mc_buffer_init(&payload, 0, 0);

	mc_message_t* message = mc_message_con_init(
		mc_message_alloc(),
		code,
		message_id,
		token,
		options,
		payload);

	uint32_t nbytes = mc_message_buffer_size(message);


	mc_buffer_t* buffer = mc_buffer_init(mc_buffer_alloc(), nbytes, ms_calloc(nbytes, uint8_t));

	mc_message_to_buffer(message, buffer);
	write_bytes(stdout, "empty", nbytes, buffer->bytes);

    CuAssert(tc, "byte[0] is 0x40", buffer->bytes[0] == 0x40);
    CuAssert(tc, "byte[1] is 0", buffer->bytes[1] == 0);
    CuAssert(tc, "byte[2] is 0", buffer->bytes[2] == 0);
    CuAssert(tc, "byte[3] is 0", buffer->bytes[3] == 0);
}

/**
 *  Given an message,
 *  When we serialize it,
 *  Then we get the expected bytes.
 */
static void test_message_to_buffer(CuTest* tc) {
	mc_buffer_t token;
	mc_options_list_t options;
	mc_buffer_t payload;
	uint8_t code = 1;
	uint16_t message_id = 2;
	uint8_t tk_value = 3;
	uint8_t pl_value = 10;

	mc_buffer_init(&token, 1, ms_copy_uint8(1, &tk_value));
	mc_options_list_vinit(&options, 1, mc_option_init_str(mc_option_alloc(), 4, "a"));
	mc_buffer_init(&payload, 1, ms_copy_uint8(1, &pl_value));

	mc_message_t* message = mc_message_con_init(
		mc_message_alloc(),
		code,
		message_id,
		token,
		options,
		payload);

	uint32_t nbytes = mc_message_buffer_size(message);
	CuAssert(tc, "size is 8", nbytes == 8);

	mc_buffer_t* buffer = mc_buffer_init(mc_buffer_alloc(), nbytes, ms_calloc(nbytes, uint8_t));

	mc_message_to_buffer(message, buffer);
	write_bytes(stdout, "mesg", nbytes, buffer->bytes);

    CuAssert(tc, "byte[0] is 0x41", buffer->bytes[0] == (uint8_t)0x41);
    CuAssert(tc, "byte[1] is 0x01", buffer->bytes[1] == (uint8_t)0x01);
    CuAssert(tc, "byte[2] is 0x00", buffer->bytes[2] == (uint8_t)0x00);
    CuAssert(tc, "byte[3] is 0x02", buffer->bytes[3] == (uint8_t)0x02);
    CuAssert(tc, "byte[4] is 0x41", buffer->bytes[4] == (uint8_t)0x41);
    CuAssert(tc, "byte[5] is 0x61", buffer->bytes[5] == (uint8_t)0x61);
    CuAssert(tc, "byte[6] is 0xff", buffer->bytes[6] == (uint8_t)0xff);
    CuAssert(tc, "byte[7] is 0x0a", buffer->bytes[7] == (uint8_t)0x0a);

    free(mc_buffer_deinit(buffer));
    free(mc_message_deinit(message));
}

/**
 *  Given a message,
 *  When we serialize it, then deserialize it,
 *  Then we get two identical messages.
 */
static void test_message_roundtrip(CuTest* tc) {
	mc_buffer_t token;
	mc_options_list_t options;
	mc_buffer_t payload;
	uint8_t code = 1;
	uint16_t message_id = 2;
	uint8_t tk_value = 3;
	uint8_t pl_value = 10;

	mc_buffer_init(&token, 1, ms_copy_uint8(1, &tk_value));
	mc_options_list_vinit(&options, 1, mc_option_init_str(mc_option_alloc(), 4, "a"));
	mc_buffer_init(&payload, 1, ms_copy_uint8(1, &pl_value));

	mc_message_t* message = mc_message_con_init(
		mc_message_alloc(),
		code,
		message_id,
		token,
		options,
		payload);

	uint32_t nbytes = mc_message_buffer_size(message);
	mc_buffer_t* buffer = mc_buffer_init(mc_buffer_alloc(), nbytes, ms_calloc(nbytes, uint8_t));

	mc_message_to_buffer(message, buffer);
	write_bytes(stdout, "mesg", nbytes, buffer->bytes);


    CuAssert(tc, "byte[0] is 0x41", buffer->bytes[0] == (uint8_t)0x41);
    CuAssert(tc, "byte[1] is 0x01", buffer->bytes[1] == (uint8_t)0x01);
    CuAssert(tc, "byte[2] is 0x00", buffer->bytes[2] == (uint8_t)0x00);
    CuAssert(tc, "byte[3] is 0x02", buffer->bytes[3] == (uint8_t)0x02);
    CuAssert(tc, "byte[4] is 0x41", buffer->bytes[4] == (uint8_t)0x41);
    CuAssert(tc, "byte[5] is 0x61", buffer->bytes[5] == (uint8_t)0x61);
    CuAssert(tc, "byte[6] is 0xff", buffer->bytes[6] == (uint8_t)0xff);
    CuAssert(tc, "byte[7] is 0x0a", buffer->bytes[7] == (uint8_t)0x0a);

    free(mc_buffer_deinit(buffer));
    free(mc_message_deinit(message));
}

/* Run all of the tests in this test suite. */
CuSuite* mc_message_suite() {
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, test_empty_message_to_buffer);
    SUITE_ADD_TEST(suite, test_message_to_buffer);
    SUITE_ADD_TEST(suite, test_message_roundtrip);
    
    return suite;
}
