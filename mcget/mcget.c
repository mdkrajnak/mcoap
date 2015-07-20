/**
 * A command line application which takes a list of coap urls and issues a GET
 * request for each resource.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mcoap/mc_uri.h"
#include "mcoap/mc_message.h"
#include "mcoap/mc_endpt_udp.h"

static void get_uri(unsigned short port, char* const uri) {
	sockaddr_t addr;
	mc_endpt_udp_t endpt;
	uint16_t msgid;

	mc_uri_to_address(&addr, uri);
	mc_endpt_udp_init(&endpt, 512, 512, "0.0.0.0", port);
	msgid = mc_endpt_udp_get(&endpt, 0, &addr, uri);

	printf("msgid: %d, uri %s\n", msgid, uri);

	// TODO IMPLEMENT
	// Read response and print payload if printable
	// Teardown endpoint
}

static void get_uris(unsigned short port, int nuri, char** uris) {
	int iuri;
	for (iuri = 0; iuri < nuri; iuri++) {
		get_uri(port, uris[iuri]);
	}
}

void usage() {
	printf(
	    "mcget [-p port] uri [uri..]\n"
		"\n"
		"mcget followed by 1 or more coap URIs\n"
		"-p port to specify the listening port\n");
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

/**
 * Pack the args vector moving values to the "left".
 * Note we place arg "1" in the 0 position on top of the
 * application name.
 */
static int packargs(int argc, char** argv) {
	int iarg;
	int idest;

	idest = 0;
	for (iarg = 1; iarg < argc; iarg++) {
		if (strcmp("-p", argv[iarg]) == 0) {
			/* Skip the port. */
			iarg++;
		}
		else {
			argv[idest] = argv[iarg];
			idest++;
		}
	}
	return idest;
}

int main(int argc, char** argv) {

	unsigned short port = MC_DEFAULT_PORT;
	int err = getport(argc, argv, &port);
	argc = packargs(argc, argv);

	if (err || (argc < 1)) usage();
	else get_uris(port, argc, argv);

	return 0;
}
