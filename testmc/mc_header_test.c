#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "msys/ms_memory.h"
#include "mcoap/mc_header.h"
#include "testmc/mc_header_test.h"

#include "cutest/CuTest.h"

/**
 *  Given all 0's for the header components
 *  When we create a header,
 *  Then the header is 0
 */
static void test_create_zero_header(CuTest* tc) {
	uint8_t version = 0;
	uint8_t message_type = 0;
	uint32_t token_len = 0;
	uint8_t code = 0;
	uint16_t message_id = 0;

    uint32_t hdr = mc_header_create(version, message_type, token_len, code, message_id);
    
    CuAssert(tc, "hdr is 0x0", hdr == 0x0);
}

/**
 *  Given all 1's (binary) for the header components
 *  When we create a header,
 *  Then the header is 0xffffffff
 */
static void test_create_ff_header(CuTest* tc) {
	uint8_t version = 0x03;
	uint8_t message_type = 0x03;
	uint32_t token_len = 0x0f;
	uint8_t code = 0xff;
	uint16_t message_id = 0xffff;

    uint32_t hdr = mc_header_create(version, message_type, token_len, code, message_id);

    CuAssert(tc, "hdr is 0xffffffff", hdr == 0xffffffff);
}

/**
 *  Given an all 0xff header
 *  When extract the components,
 *  Then we get the appropriate number given the number of bits in the field.
 */
static void test_get_ff_header(CuTest* tc) {
	uint32_t hdr = 0xffffffff;

	uint8_t ex_version = 0x03;
	uint8_t ex_message_type = 0x03;
	uint32_t ex_token_len = 0x0f;
	uint8_t ex_code = 0xff;
	uint16_t ex_message_id = 0xffff;

    CuAssert(tc, "version is 0x03", mc_header_get_version(hdr) == ex_version);
    CuAssert(tc, "message_type is 0x03", mc_header_get_message_type(hdr) == ex_message_type);
    CuAssert(tc, "token_len is 0x0f", mc_header_get_token_length(hdr) == ex_token_len);
    CuAssert(tc, "code is 0xff", mc_header_get_code(hdr) == ex_code);
    CuAssert(tc, "message_id is 0xffff", mc_header_get_message_id(hdr) == ex_message_id);
}

/**
 *  Given an all 0x55 header
 *  When extract the components,
 *  Then we get the appropriate number given the number of bits in the field.
 */
static void test_get_55_header(CuTest* tc) {
	uint32_t hdr = 0x55555555;

	uint8_t ex_version = 0x01;
	uint8_t ex_message_type = 0x01;
	uint32_t ex_token_len = 0x05;
	uint8_t ex_code = 0x55;
	uint16_t ex_message_id = 0x5555;

    CuAssert(tc, "version is 0x01", mc_header_get_version(hdr) == ex_version);
    CuAssert(tc, "message_type is 0x01", mc_header_get_message_type(hdr) == ex_message_type);
    CuAssert(tc, "token_len is 0x05", mc_header_get_token_length(hdr) == ex_token_len);
    CuAssert(tc, "code is 0x55", mc_header_get_code(hdr) == ex_code);
    CuAssert(tc, "message_id is 0x5555", mc_header_get_message_id(hdr) == ex_message_id);
}

/* Run all of the tests in this test suite. */
CuSuite* mc_header_suite() {
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, test_create_zero_header);
    SUITE_ADD_TEST(suite, test_create_ff_header);
    SUITE_ADD_TEST(suite, test_get_ff_header);
    SUITE_ADD_TEST(suite, test_get_55_header);

    return suite;
}
