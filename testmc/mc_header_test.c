#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mcoap/mc_header.h"
#include "testmc/mc_header_test.h"

#include "cutest/CuTest.h"

/**
 *  Given a code of 1 and no detail
 *  When we create a code
 *  Then the code is 0x20 and the category is 1.
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

/* Run all of the tests in this test suite. */
CuSuite* mc_header_suite() {
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, test_create_zero_header);
        
    return suite;
}
