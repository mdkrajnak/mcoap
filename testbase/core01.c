#include <stdlib.h>

#include "msys/ms_memory.h"
#include "msys/ms_log.h"
#include "mcoap/mc_header.h"
#include "mcoap/mc_options_list.h"
#include "mcoap/mc_uri.h"
#include "mcoap/mc_code.h"
#include "mcoap/mc_message.h"
#include "mcoap/mc_endpt_udp.h"

#include "testbase/core01.h"
#include "testbase/print.h"
#include "testbase/compare.h"

#include "accessor.h"

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

static mc_option_t* get_option(mc_options_list_t* list, uint16_t num) {
    int index = mc_options_list_get_index(list, 0, num);
    return mc_options_list_get(list, index);
}

static int result_fn(mc_endpt_id_t endpt, uint16_t msgid, int status) {
    (void)endpt;
    (void)msgid;
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
    mc_buffer_t* token;
    mc_message_t* msg;
    uint8_t code;
    mc_option_t* cformat;

    unsigned short port = 5683;
    char* const uri = "coap://coap.me:5683/test";

    mc_uri_to_address(&addr, uri);
    mc_endpt_udp_init(&endpt, 1024, 1024, "0.0.0.0", port);

    msgid = mc_endpt_udp_get(&endpt, &addr, result_fn, uri, 0);
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
    CuAssert(tc, "Code detail is 5", mc_code_get_detail(code) == 5);

    CuAssert(tc, "Received message id should match sent id", mc_header_get_message_id(msg->header) == msgid);
    CuAssert(tc, "Received token should match sent token", mc_buffer_eq(msg->token, token));

    cformat = get_option(msg->options, OPTION_CONTENT_FORMAT);
    CuAssert(tc, "Format option should exist", cformat != 0);

    if (token) ms_free(mc_buffer_deinit(token));
    if (msg) ms_free(mc_message_deinit(msg));
    mc_endpt_udp_deinit(&endpt);
}

/* Run all of the tests in this test suite. */
CuSuite* core01_suite() {
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, test_client_get_con_mode);

    return suite;
}
