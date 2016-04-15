/**
 * A simple time server oriented to benchmarking.
 * It treates the payload as an array of uint64_t's.
 * The first value is supplied by the client, for example
 * a sequence number. The next two values are the sent time in 
 * seconds since epoch and nanoseconds into the second.
 * 
 * The server fills in elements 3 and 4 of the array with the 
 * receipt time on the server.
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

 int request_handler(mc_endpt_udp_t* const endpt, mc_message_t* const msg) {
    // Currently just echoing the message.
    if (msg->from) mc_endpt_udp_send(endpt, msg->from, msg, 0);
    return 1;
 }
 
static void run_server(unsigned short port) {
    sockaddr_t addr;
    mc_endpt_udp_t endpt;

    mc_endpt_udp_init(&endpt, 1024, 1024, "0.0.0.0", port);
    mc_endpt_udp_loop(&endpt, request_handler);
    mc_endpt_udp_deinit(&endpt);
}

void usage() {
    printf(
        "tmserver [-p port]\n"
        "\n"
        "-s port to specify the server port\n");
}

static int getport(int argc, char** argv, unsigned short* port) {
    int iarg;
    int err;

    /* Note the endpoints, skip the first and last. */
    for (iarg = 1; iarg < (argc - 1); iarg++) {
        if (strcmp("-s", argv[iarg]) == 0) {
            *port = atoi(argv[iarg+1]);
            if (*port == 0) {
                printf("Unable to parse port %s.\n", argv[iarg+1]);
                err = 1;
            }
            else {
                err = 0;
            }
            return err;
        }
    }

    /* Check the case of a trailing -p option. */
    if (strcmp("-s", argv[argc-1]) == 0) {
        printf("Trailing -s without a port number.\n");
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

    unsigned short port = MC_DEFAULT_PORT;
    int err = getport(argc, argv, &port);
    
    if (err) usage();
    else run_server(port);

    return 0;
}
