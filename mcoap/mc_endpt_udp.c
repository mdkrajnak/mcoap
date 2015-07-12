/**
 * @file
 * @ingroup endpt_udp
 * @{
 */

#include "msys/ms_memory.h"
#include "msys/ms_log.h"
#include "mnet/mn_timeout.h"
#include "mcoap/mc_endpt_udp.h"

mc_endpt_udp_t* mc_endpt_udp_alloc() {
	return ms_calloc(1, mc_endpt_udp_t);
}

mc_endpt_udp_t* mc_endpt_udp_init(mc_endpt_udp_t* endpt, size_t bufsize, const char* hostname, unsigned short port) {
	sockaddr_t addr;

	endpt->bufsize = bufsize;
	endpt->readfn = 0;
	endpt->thread = 0;
	endpt->running = 0;

	if (hostname == 0) {
		ms_log_debug("Hostname is null");
		return 0;
	}

	if (!mn_inetaddr_init(&addr, hostname, port)) {
		ms_log_debug("Failed to initialize address %s:%d", hostname, port);
		return 0;
	}

	if (mn_socket_create(&endpt->sock, AF_INET, SOCK_DGRAM, 0) != MN_DONE) {
		ms_log_debug("Failed to create socket.");
		return 0;
	}

	if (mn_socket_bind(&endpt->sock, &addr, sizeof(sockaddr_t)) != MN_DONE) {
		ms_log_debug("Failed to bind socket on %s:%d", hostname, port);
		return 0;
	}

	return endpt;
}

mc_endpt_udp_t* mc_endpt_udp_deinit(mc_endpt_udp_t* endpt) {
	return endpt;
}

static void endpt_udp_reader(void* data) {
	mn_timeout_t tout;
	sockaddr_t fromaddr;
	socklen_t addrlen;
	mc_endpt_udp_t* endpt;
	size_t got;
	char* buffer;

	mn_timeout_init(&tout, 0, 0);
	endpt = (mc_endpt_udp_t*)data;
	buffer = ms_calloc(endpt->bufsize, char);

	mn_socket_recvfrom(&endpt->sock, buffer, endpt->bufsize, &got, &fromaddr, &addrlen, &tout);
}

int mc_endpt_udp_start(mc_endpt_udp_t* endpt, mc_endpt_read_fn_t readfn) {
	endpt->readfn = readfn;
	endpt->thread = ms_thread_init(ms_thread_alloc(), (ms_thread_fn_t*)&endpt_udp_reader, endpt);

	return 0;
}

int mc_endpt_udp_stop(mc_endpt_udp_t* endpt) {
	return 0;
}

int mc_endpt_udp_write(mc_endpt_udp_t* endpt, mc_message_t* msg) {
	return 0;
}

/** @} */
