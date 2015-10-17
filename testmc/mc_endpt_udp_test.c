#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "msys/ms_memory.h"
#include "mcoap/mc_code.h"
#include "mcoap/mc_uri.h"

#include "mcoap/mc_buffer_queue.h"
#include "mcoap/mc_endpt_udp.h"
#include "testmc/mc_endpt_udp_test.h"

#include "cutest/CuTest.h"

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

    amsgid = mc_endpt_udp_get(&alice, &addr, 0, uri);
    amsg = mc_endpt_udp_recv(&bob);

    CuAssert(tc, "msg received",  amsg != 0);

    bmsgid = mc_message_get_message_id(amsg);

    print_msg(amsg);

    if (amsg) ms_free(mc_message_deinit(amsg));
    mc_endpt_udp_deinit(&alice);
    mc_endpt_udp_deinit(&bob);

    CuAssert(tc, "msgid's are equal",  amsgid == bmsgid);
}

 int test_result_fn(mc_endpt_id_t endpt, uint16_t msgid, int status);

/**
 *  Given one endpoints,
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

    amsgid = mc_endpt_udp_get(&alice, &addr, 0, uri);

    ctr = alice.confirmq.first->xmitcounter;
    CuAssert(tc, "message is enqueued", amsgid != 0);
    CuAssert(tc, "message is enqueued", ctr == 0);


    mc_endpt_udp_check_queues(&alice);

    mc_endpt_udp_deinit(&alice);

    CuAssert(tc, "msgid's are equal", 0);

}

/* Run all of the tests in this test suite. */
CuSuite* mc_endpt_udp_suite() {
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, test_send_recv);
    SUITE_ADD_TEST(suite, test_rexmit_con_msg);
        
    return suite;
}
