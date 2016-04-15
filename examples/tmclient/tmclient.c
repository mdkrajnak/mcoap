/**
 * A command line application which takes a list of coap urls and issues a GET
 * request for each resource.
 */

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

static mc_buffer_t* mk_payload(int npacket) {
    uint64_t* payload = ms_calloc(32, uint64_t);
    payload[0] = npacket;
    
    return mc_buffer_init(mc_buffer_alloc(), 256, (uint8_t*)payload);
}

static void process_resp(mc_message_t* const msg) {
    if (msg == 0) return;
    uint64_t* payload = (uint64_t*)msg->payload->bytes;
    
    printf("Received message %ld with %d bytes\n", payload[0], msg->payload->nbytes);
}

static void run_client(unsigned short cltport, unsigned short srvport, unsigned short ntimes) {
    sockaddr_t addr;
    mc_endpt_udp_t endpt;
    uint16_t msgid;
    mc_message_t* msg;
    uint8_t* payload;
    unsigned short itimes;
    double start;
    double stop;
    char* uri = ms_calloc(64, char);
    
    sprintf(uri, "coap://127.0.0.1:%u/gettime", srvport);
    
    mc_uri_to_address(&addr, uri);
    mc_endpt_udp_init(&endpt, 1024, 1024, "0.0.0.0", cltport);

    msg = 0;
    start = mn_gettime();
    for (itimes = 0; itimes < ntimes; itimes++) {
        // Note we pass in 0 for the result function so the message is nonconfirmable.
        msgid = mc_endpt_udp_post(&endpt, &addr, 0, uri, 0, mk_payload(itimes));
        msg = mc_endpt_udp_recv(&endpt);
        process_resp(msg);
        if (msg) ms_free(mc_message_deinit(msg));
    }
    stop = mn_gettime();
    printf("elapsed: %g, %g kb/sec\n", (stop - start), ((ntimes * (256.0/1024.0))/(stop - start)));

    ms_free(uri);
    mc_endpt_udp_deinit(&endpt);
}

void usage() {
    printf(
        "tmclient [-p port]\n"
        "\n"
        "-c port to specify the client port\n"
        "-s port to specify the server port\n"
        "-n to specify the number of requests\n");
}

static int getport(int argc, char** argv, char* const flag, unsigned short* port) {
    int iarg;
    int err;

    /* Note the endpoints, skip the first and last. */
    for (iarg = 1; iarg < (argc - 1); iarg++) {
        if (strcmp(flag, argv[iarg]) == 0) {
            *port = atoi(argv[iarg+1]);
            if (*port == 0) {
                printf("Unable to parse %s.\n", argv[iarg+1]);
                err = 1;
            }
            else {
                err = 0;
            }
            return err;
        }
    }

    /* Check the case of a trailing option. */
    if (strcmp(flag, argv[argc-1]) == 0) {
        printf("Trailing %s without a number.\n", flag);
        err = 1;
    }
    else {
        err = 0;
    }
    /* No option found, leave the default. */
    return err;
}

int main(int argc, char** argv) {
    ms_log_setfile(stdout);
    ms_log_setlevel(ms_debug);

    unsigned short cltport = MC_DEFAULT_PORT+1;
    unsigned short srvport = MC_DEFAULT_PORT;
    unsigned short ntimes = 10;
    int err = getport(argc, argv, "-c", &cltport);
    if (!err) err = getport(argc, argv, "-s", &srvport);
    if (!err) err = getport(argc, argv, "-n", &ntimes);

    if (err) usage();
    else run_client(cltport, srvport, ntimes);

    return 0;
}
