#include <stdlib.h>

#include "msys/ms_memory.h"
#include "msys/ms_log.h"
#include "mcoap/mc_header.h"
#include "mcoap/mc_options_list.h"
#include "mcoap/mc_uri.h"
#include "mcoap/mc_code.h"
#include "mcoap/mc_message.h"
#include "mcoap/mc_endpt_udp.h"

#include "core02_delete_con.h"
#include "testbase/print.h"
#include "testbase/compare.h"

#include "accessor.h"

/**
 * Check server sends response containing:
 *
 * * Code = 2.02 (Deleted)
 * * Message ID = CMID, Token = CTOK
 * * Content-format option if payload non-empty
 * * Empty or non-empty Payload
 */

static int completed_status;
static int completed = 0;

static int result_fn(mc_endpt_id_t endpt, uint16_t msgid, int status) {
    (void)endpt;
    (void)msgid;
    completed = 1;
    completed_status = status;

    return 0;
}

/**
*  Given a test server,
*  when send a DELETE in CON mode,
*  then we get the correct response.
*/
static void test_delete_con(CuTest* tc) {
    sockaddr_t addr;
    mc_endpt_udp_t endpt;
    uint16_t msgid;
    mc_buffer_t* token;
    mc_message_t* msg;
    uint8_t code;
    mc_option_t* cformat;

    unsigned short port = 5683;
    char* const uri = "coap://coap.me:5683/test";

    printf("test_delete_con()\n");

    mc_uri_to_address(&addr, uri);
    mc_endpt_udp_init(&endpt, 1024, 1024, "0.0.0.0", port);

    msgid = mc_endpt_udp_delete(&endpt, &addr, result_fn, uri, 0);
    token = mc_endpt_udp_copy_queued_token(&endpt, msgid);

    msg = 0;
    while ((msg == 0) && (completed == 0)) {
        msg = mc_endpt_udp_recv(&endpt);
        mc_endpt_udp_check_queues(&endpt);
    }

    // Check test conditions.
    CuAssert(tc, "Message should not be null", msg != 0);

    print_msg(msg);

    code = mc_message_get_code(msg);
    CuAssert(tc, "Code category is 2", mc_code_get_category(code) == 2);
    CuAssert(tc, "Code detail is 2", mc_code_get_detail(code) == 2);

    CuAssert(tc, "Received message id should match sent id", mc_header_get_message_id(msg->header) == msgid);
    CuAssert(tc, "Received token should match sent token", mc_buffer_eq(msg->token, token));

    if (msg->payload != 0) {
        ms_log_debug("The payload is non-null");
        ms_log_debug("payload: %.*s", msg->payload->nbytes, msg->payload->bytes);

        cformat = mc_options_list_get(msg->options, OPTION_CONTENT_FORMAT);
        CuAssert(tc, "Format option should exist", cformat != 0);
    }
    else {
        ms_log_debug("The payload is null");
    }

    if (token) ms_free(mc_buffer_deinit(token));
    if (msg) ms_free(mc_message_deinit(msg));
    mc_endpt_udp_deinit(&endpt);
}

/* Run all of the tests in this test suite. */
CuSuite* core02_suite() {
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, test_delete_con);

    return suite;
}
