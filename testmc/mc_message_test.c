#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "msys/ms_memory.h"
#include "mcoap/mc_message.h"
#include "testmc/mc_message_test.h"

#include "cutest/CuTest.h"

/* @todo extract to common utilities. */
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
 *  Given
 *  When
 *  Then we get the appropriate number given the number of bits in the field.
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
	write_bytes(stdout, nbytes, buffer->bytes);

    CuAssert(tc, "byte[0] is 0", buffer->bytes[0] == 0);
    CuAssert(tc, "byte[1] is 0", buffer->bytes[1] == 0);
    CuAssert(tc, "byte[2] is 0", buffer->bytes[2] == 0);
    CuAssert(tc, "byte[3] is 0", buffer->bytes[3] == 0);
}

/* Run all of the tests in this test suite. */
CuSuite* mc_message_suite() {
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, test_empty_message_to_buffer);

    return suite;
}
