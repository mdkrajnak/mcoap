#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mcoap/mc_code.h"
#include "testmc/mc_code_test.h"

#include "cutest/CuTest.h"

/**
 *  Given a code of 1 and no detail
 *  When we create a code
 *  Then the code is 0x20 and the category is 1.
 */
static void test_create_category(CuTest* tc) {
    uint8_t code = 0;
    
    CuAssert(tc, "category is 0", mc_code_get_category(code) == 0);
    CuAssert(tc, "detail is 0", mc_code_get_detail(code) == 0);
        
    code = mc_code_create(1,0);
    
    CuAssert(tc, "category is 1", mc_code_get_category(code) == 1);
    CuAssert(tc, "detail is 0", mc_code_get_detail(code) == 0);
    CuAssert(tc, "code is 0x20",  code == 0x20);
}

/**
 *  Given a code of 0 and a detail of 1
 *  When we create a code
 *  Then the code is 1 and the category is 0.
 */
static void test_create_detail(CuTest* tc) {
    uint8_t code = 0;
    
    code = mc_code_create(0, 1);
    
    CuAssert(tc, "category is 0", mc_code_get_category(code) == 0);
    CuAssert(tc, "detail is 1", mc_code_get_detail(code) == 1);
    CuAssert(tc, "code is 0x01",  code == 0x01);
}

/**
 *  Given a code of 1 and detail of 2
 *  When we create a code
 *  Then the code is 0x22 and the category is 1 and te detail is 2.
 */
static void test_create_code(CuTest* tc) {
    uint8_t code = 0;
        
    code = mc_code_create(1,2);
    
    CuAssert(tc, "category is 1", mc_code_get_category(code) == 1);
    CuAssert(tc, "detail is 2", mc_code_get_detail(code) == 2);
    CuAssert(tc, "code is 0x22",  code == 0x22);
}

/* Run all of the tests in this test suite. */
CuSuite* mc_code_suite() {
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, test_create_category);
    SUITE_ADD_TEST(suite, test_create_detail);
    SUITE_ADD_TEST(suite, test_create_code);
        
    return suite;
}
