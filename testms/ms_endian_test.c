#include <stdio.h>

#include "msys/ms_endian.h"

#include "testms/ms_endian_test.h"

static void test_swap16(CuTest* tc) {
    printf("running test_swap16\n");

    CuAssert(tc, "", ms_swap_u16(0x0102) == 0x0201);
}

static void test_swap32(CuTest* tc) {
    printf("running test_swap32\n");

    CuAssert(tc, "", ms_swap_u32(0x01020304) == 0x04030201);
}

static void test_swap64(CuTest* tc) {
    uint64_t temp;

    printf("running test_swap64\n");

    CuAssert(tc, "", ms_swap_u64(0x0102030405060708ULL) == 0x0807060504030201ULL);

    temp = 0x0102030405060708ULL;

    if (ms_is_littleendian()) {
        ms_hton64(&temp);
        CuAssert(tc, "", temp == 0x0807060504030201ULL);
    }
    else {
        ms_ntoh64(&temp);
        CuAssert(tc, "", temp == 0x0102030405060708ULL);
    }
}

static void test_system_endian(CuTest* tc) {
    uint64_t temp;

    printf("running test_swap64\n");

    CuAssert(tc, "", ms_swap_u64(0x0102030405060708ULL) == 0x0807060504030201ULL);

    temp = 0x0102030405060708ULL;

    ms_ntoh64(&temp);
    if(temp != 0x0102030405060708ULL)
    {
        printf("running system is little endian\n");
    }else{
        printf("running system is big endian\n");
    }
}

/* Run all of the tests in this test suite. */
CuSuite* ms_endian_suite() {
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, test_swap16);
    SUITE_ADD_TEST(suite, test_swap32);
    SUITE_ADD_TEST(suite, test_swap64);
    SUITE_ADD_TEST(suite, test_system_endian);

    return suite;
}
