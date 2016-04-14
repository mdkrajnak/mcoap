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

static void process_resp(mc_message_t* const msg) {
}

static void run_client(unsigned short port, int ntimes) {
    sockaddr_t addr;
    mc_endpt_udp_t endpt;
    uint16_t msgid;
    mc_message_t* msg;
    int itimes = 0;
    char* uri = "coap://127.0.0.1:5268/gettime";

    mc_uri_to_address(&addr, uri);
    mc_endpt_udp_init(&endpt, 1024, 1024, "0.0.0.0", port);

    msg = 0;
    for (itimes = 0; itimes < ntimes; itimes++) {
        // Note we pass in 0 for the result function so the message is nonconfirmable.
        msgid = mc_endpt_udp_get(&endpt, &addr, 0, uri, 0);
        msg = mc_endpt_udp_recv(&endpt);
        process_resp(msg);
        if (msg) ms_free(mc_message_deinit(msg));
    }

    mc_endpt_udp_deinit(&endpt);
}

void usage() {
    printf(
        "tmclient [-p port]\n"
        "\n"
        "-p port to specify the receiving port\n");
}

static int getport(int argc, char** argv, unsigned short* port) {
    int iarg;
    int err;

    /* Note the endpoints, skip the first and last. */
    for (iarg = 1; iarg < (argc - 1); iarg++) {
        if (strcmp("-p", argv[iarg]) == 0) {
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
    if (strcmp("-p", argv[argc-1]) == 0) {
        printf("Trailing -p without a port number.\n");
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
    else run_client(port, 10);

    return 0;
}
