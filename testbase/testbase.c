#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cutest/CuTest.h"
#include "msys/ms_log.h"

#include "testbase/core01_get_con.h"
#include "testbase/core02_delete_con.h"


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

    add_tmp_suite(suite, core01_suite());
    add_tmp_suite(suite, core02_suite());

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
