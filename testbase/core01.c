#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "msys/ms_memory.h"
#include "msys/ms_log.h"
#include "mnet/mn_timeout.h"
#include "mcoap/mc_uri.h"
#include "mcoap/mc_code.h"
#include "mcoap/mc_message.h"
#include "mcoap/mc_endpt_udp.h"

#include "mcoap/mc_header.h"
#include "testbase/core01.h"

#include "cutest/CuTest.h"

/**
 * Check that the server sends response containing:
 *
 *  * Code = 2.05 (Content)
 *  * Message ID = CMID, Token = CTOK
 *  * Content-format option
 *  * Non-empty Payload
 */

int completed_status;
int completed = 0;

static int result_fn(mc_endpt_id_t endpt, uint16_t msgid, int status) {
    completed = 1;
    completed_status = status;

    return 0;
}

/**
 *  Given all 0's for the header components
 *  When we create a header,
 *  Then the header is 0
 */
static void test_client_get_con_mode(CuTest* tc) {
    sockaddr_t addr;
    mc_endpt_udp_t endpt;
    uint16_t msgid;
    mc_message_t* msg;

    unsigned short port = 1000;
    char* const uri = "coap://coap.me/test";

    mc_uri_to_address(&addr, uri);
    mc_endpt_udp_init(&endpt, 1024, 1024, "0.0.0.0", port);

    msgid = mc_endpt_udp_get(&endpt, &addr, result_fn, uri, 0);

    msg = 0;
    while ((msg == 0) && (completed == 0)) {
        msg = mc_endpt_udp_recv(&endpt);
        mc_endpt_udp_check_queues(&endpt);
    }

    // Check test conditions.

    // TBD

    if (msg) ms_free(mc_message_deinit(msg));
    mc_endpt_udp_deinit(&endpt);
}

/* Run all of the tests in this test suite. */
CuSuite* core01_suite() {
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, test_client_get_con_mode);

    return suite;
}
