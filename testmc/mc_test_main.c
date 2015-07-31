#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cutest/CuTest.h"
#include "msys/ms_log.h"

#include "testmc/mc_code_test.h"
#include "testmc/mc_header_test.h"
#include "testmc/mc_options_list_test.h"
#include "testmc/mc_message_test.h"
#include "testmc/mc_uri_test.h"
#include "testmc/mc_endpt_udp_test.h"

#if defined(WIN32) && defined(_DEBUG)
void dumpMemLeaks() {
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDOUT);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDOUT);
    _CrtDumpMemoryLeaks();
}
#else
void dumpMemLeaks() {
}
#endif

static void add_tmp_suite(CuSuite* primary, CuSuite* secondary) {
    CuSuiteAddSuite(primary, secondary);
    free(secondary);
}

void run_all_tests() {
    CuString *summary = CuStringNew();
    CuSuite* suite = CuSuiteNew();

    ms_log_setlevel(ms_debug);
    ms_log_setfile(stderr);

    ms_log_debug("starting testing");

    add_tmp_suite(suite, mc_code_suite());
    add_tmp_suite(suite, mc_header_suite());
    add_tmp_suite(suite, mc_options_list_suite());
    add_tmp_suite(suite, mc_message_suite());
    add_tmp_suite(suite, mc_uri_suite());
    add_tmp_suite(suite, mc_endpt_udp_suite());

    CuSuiteRun(suite);
    
    CuSuiteSummary(suite, summary);
    CuSuiteDetails(suite, summary);
    printf("%s\n", summary->buffer);   
    
    CuStringDelete(summary);
    CuSuiteDelete(suite);
}

int main(int argc, char** argv) {
    run_all_tests();

    if (argc > 1 && strcmp("-leaks", argv[1]) == 0) {
        dumpMemLeaks();
    }

    return 0;
}
