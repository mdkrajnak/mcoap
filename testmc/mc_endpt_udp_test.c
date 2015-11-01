#include <stdio.h>
#include <stdlib.h>

#include "msys/ms_memory.h"
#include "mcoap/mc_code.h"
#include "mcoap/mc_uri.h"

#include "mcoap/mc_buffer_queue.h"
#include "mcoap/mc_endpt_udp.h"
#include "testmc/mc_endpt_udp_test.h"

static void print_token(mc_buffer_t* token) {
    uint32_t ibyte;

    printf("token:    ");
    for (ibyte = 0; ibyte < token->nbytes; ibyte++) {
        printf("%x", token->bytes[ibyte]);
    }
    printf("\n");
}

static void print_msg(mc_message_t* const msg) {
    if (msg == 0) {
        printf("no response received\n");
    }
    else {
        uint8_t code = mc_message_get_code(msg);

        printf("version:  %d.\n", mc_message_get_version(msg));
        printf("type:     %d.\n", mc_message_get_type(msg));
        printf("msgid:    %d.\n", mc_message_get_message_id(msg));
        print_token(msg->token);
        printf("category: %d.\n", mc_code_get_category(code));
        printf("detail:   %d.\n", mc_code_get_category(code));

        /** @todo check content type. */
        if (msg->payload) printf("%.*s\n", msg->payload->nbytes, msg->payload->bytes);
    }
}

static int msg_is_in_queue(mc_endpt_udp_t* endpt, uint16_t msgid) {
    mc_buffer_queue_entry_t* current;

    current = endpt->confirmq.first;
    while (current) {
        if (current->msgid == msgid) return 1;
        current = current->next;
    }

    return 0;
}

/**
 *  Given two endpoints: alice and bob,
 *  when we send a get message from alice to bob,
 *  then bob receives alice's message.
 */
static void test_send_recv(CuTest* tc) {
    sockaddr_t addr;
    mc_endpt_udp_t alice;
    mc_endpt_udp_t bob;
    uint16_t amsgid;
    uint16_t bmsgid;
    mc_message_t* amsg;
    char* uri = "coap://localhost:5679/test";
    uint16_t aport = 5678;
    uint16_t bport = 5679;

    mc_uri_to_address(&addr, uri);
    mc_endpt_udp_init(&alice, 512, 512, "0.0.0.0", aport);
    mc_endpt_udp_init(&bob, 512, 512, "0.0.0.0", bport);

    amsgid = mc_endpt_udp_get(&alice, &addr, 0, uri, 0);
    amsg = mc_endpt_udp_recv(&bob);

    CuAssert(tc, "msg received",  amsg != 0);

    bmsgid = mc_message_get_message_id(amsg);

    print_msg(amsg);

    if (amsg) ms_free(mc_message_deinit(amsg));
    mc_endpt_udp_deinit(&alice);
    mc_endpt_udp_deinit(&bob);

    CuAssert(tc, "msgid's are equal",  amsgid == bmsgid);
}

static int test_status;
static uint16_t test_msgid;

int test_result_fn(mc_endpt_id_t endpt, uint16_t msgid, int status) {
    test_msgid = msgid;
    test_status = status;

    printf("test_result_fn() called. endpt %p, msgid: %d, status %d\n", endpt, msgid, status);
    return 0;
}

/**
 * Compute a new start value that will force the check queue function to timeout.
 */
static double recompute_start(mn_timeout_t *tmout) {
    static double epsilon = 0.01;
    return tmout->start - tmout->total - epsilon;
}

/**
 *  Given one endpoint,
 *  when we send a message and it's not ack'd,
 *  then it is retransmitted when we check the queues for timeouts.
 */
static void test_rexmit_con_msg(CuTest* tc) {
    sockaddr_t addr;
    mc_endpt_udp_t alice;
    uint16_t amsgid;
    char* uri = "coap://localhost:5679/test";
    uint16_t aport = 5678;
    int ctr;

    mc_uri_to_address(&addr, uri);
    mc_endpt_udp_init(&alice, 512, 512, "0.0.0.0", aport);

    amsgid = mc_endpt_udp_get(&alice, &addr, test_result_fn, uri, 0);

    ctr = alice.confirmq.first->xmitcounter;
    CuAssert(tc, "message is enqueued", msg_is_in_queue(&alice, amsgid));
    CuAssert(tc, "msg was sent once", ctr == 1);

    // Update the timeout entry to force a retransmission.
    alice.confirmq.first->timeout.start = recompute_start(&alice.confirmq.first->timeout);

    mc_endpt_udp_check_queues(&alice);
    ctr = alice.confirmq.first->xmitcounter;

    // Cleanup
    mc_endpt_udp_deinit(&alice);

    CuAssert(tc, "message was sent twice", ctr == 2);
}

/**
 *  Given one endpoint,
 *  when retransmit a message up to its retransmission limit,
 *  then the result function is called with a timeout status.
 */
static void test_max_rexmit_con_msg(CuTest* tc) {
    sockaddr_t addr;
    mc_endpt_udp_t alice;
    uint16_t amsgid;
    char* uri = "coap://localhost:5679/test";
    uint16_t aport = 5678;

    /* Set the test flags. */
    test_status = MN_DONE;
    test_msgid = 999;

    mc_uri_to_address(&addr, uri);
    mc_endpt_udp_init(&alice, 512, 512, "0.0.0.0", aport);

    amsgid = mc_endpt_udp_get(&alice, &addr, test_result_fn, uri, 0);

    do {
        // Update the timeout entry to force a retransmission.
        alice.confirmq.first->timeout.start = recompute_start(&alice.confirmq.first->timeout);
        mc_endpt_udp_check_queues(&alice);
    } while (alice.confirmq.first && (alice.confirmq.first->xmitcounter <= MAX_RETRANSMIT));

    /* Check the test flags. */
    printf("test_status: %d\n", test_status);
    CuAssert(tc, "xmit timed out", test_status == MN_TIMEOUT);
    CuAssert(tc, "notified of timeout out msg", test_msgid == amsgid);

    // Cleanup
    mc_endpt_udp_deinit(&alice);
}

/**
 *  Given two endpoints,
 *  when we send a message and it is ack'd,
 *  then it is removed from alices retransmit queue.
 *  NOTE! Is the result function called?
 */
static void test_send_ack(CuTest* tc) {
    sockaddr_t alice_addr;
    sockaddr_t bob_addr;
    mc_endpt_udp_t alice;
    mc_endpt_udp_t bob;
    uint16_t amsgid;
    mc_message_t* amsg;
    mc_message_t* bmsg;
    mc_buffer_t* resp_token;

    char* alice_uri = "coap://localhost:5678/test";
    char* bob_uri = "coap://localhost:5679/test";
    uint16_t aport = 5678;
    uint16_t bport = 5679;

    mc_uri_to_address(&alice_addr, alice_uri);
    mc_uri_to_address(&bob_addr, bob_uri);

    mc_endpt_udp_init(&alice, 512, 512, "0.0.0.0", aport);
    mc_endpt_udp_init(&bob, 512, 512, "0.0.0.0", bport);

    amsgid = mc_endpt_udp_get(&alice, &bob_addr, test_result_fn, bob_uri, 0);
    CuAssert(tc, "message is enqueued", msg_is_in_queue(&alice, amsgid));

    amsg = mc_endpt_udp_recv(&bob);
    resp_token = mc_message_copy_token(amsg);
    if (amsg) {
        ms_free(mc_message_deinit(amsg));
    }

    mc_endpt_udp_ack(&bob, &alice_addr, resp_token, amsgid);

    /* Set the test flags. */
    test_status = MN_DONE;
    test_msgid = 999;

    bmsg = mc_endpt_udp_recv(&alice);
    if (bmsg) {
        ms_free(mc_message_deinit(bmsg));
    }

    CuAssert(tc, "message is dequeued", msg_is_in_queue(&alice, amsgid) == 0);

    // @todo Do we need to check to see if result fn called for sender?
    mc_endpt_udp_deinit(&alice);
    mc_endpt_udp_deinit(&bob);

    CuAssert(tc, "msg status was acked", test_status == MN_DONE);
    CuAssert(tc, "msg id was acked", test_msgid == amsgid);
}


/* Run all of the tests in this test suite. */
CuSuite* mc_endpt_udp_suite() {
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, test_send_recv);
    SUITE_ADD_TEST(suite, test_rexmit_con_msg);
    SUITE_ADD_TEST(suite, test_max_rexmit_con_msg);
    SUITE_ADD_TEST(suite, test_send_ack);

    return suite;
}
