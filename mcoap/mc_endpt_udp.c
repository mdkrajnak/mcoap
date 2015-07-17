/**
 * @file
 * @ingroup endpt_udp
 * @{
 */

#include "msys/ms_config.h"
#include "msys/ms_memory.h"
#include "msys/ms_log.h"
#include "mnet/mn_timeout.h"
#include "mcoap/mc_endpt_udp.h"
#include "mcoap/mc_message.h"

#define DEFAULT_ENDPT_TIMEOUT 0.2

mc_endpt_udp_t* mc_endpt_udp_alloc() {
	return ms_calloc(1, mc_endpt_udp_t);
}

mc_endpt_udp_t* mc_endpt_udp_init(mc_endpt_udp_t* const endpt, uint32_t rdsize, uint32_t wrsize, const char* hostname, unsigned short port) {
	sockaddr_t addr;

	endpt->readfn = 0;
	endpt->thread = 0;
	endpt->running = 0;

	/* @todo consider restricting the maxmimum buffer sizes (e.g. less then 64k). */
	mc_buffer_init(&endpt->rdbuffer, rdsize, ms_calloc(rdsize, uint8_t));
	mc_buffer_init(&endpt->wrbuffer, wrsize, ms_calloc(wrsize, uint8_t));

	mn_timeout_init(&endpt->tmout, DEFAULT_ENDPT_TIMEOUT, -1.0);
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

mc_endpt_udp_t* mc_endpt_set_timeout(mc_endpt_udp_t* const endpt, double seconds) {
	mn_timeout_init(&endpt->tmout, seconds, -1.0);
	return endpt;
}

mc_endpt_udp_t* mc_endpt_udp_deinit(mc_endpt_udp_t* const endpt) {
	mc_buffer_deinit(&endpt->rdbuffer);
	mc_buffer_deinit(&endpt->wrbuffer);

	return endpt;
}

static void endpt_udp_reader(void* data) {
	mn_timeout_t tout;
	sockaddr_t fromaddr;
	socklen_t addrlen;
	mc_endpt_udp_t* rendpt;
	uint32_t rdsize;
	uint32_t bpos;

	/* Initialize variables. */
	/* Note we need to save the original buffer size and reset for each read. */
	mn_timeout_init(&tout, 0.2, -0.2);
	rendpt = (mc_endpt_udp_t*)data;
	rdsize = rendpt->rdbuffer.nbytes;

	while (rendpt->running) {
		/* Allocate and read a message. */
		mc_message_t* msg = mc_message_alloc();
		mn_socket_recvfrom(&rendpt->sock, (char*)rendpt->rdbuffer.bytes, rendpt->rdbuffer.nbytes, (size_t*)&rendpt->rdbuffer.nbytes, &fromaddr, &addrlen, &tout);

		/* Parse and handle the message, then cleanup. */
		bpos = 0;
		msg = mc_message_from_buffer(msg, &rendpt->rdbuffer, &bpos);
		rendpt->readfn(&fromaddr, msg);
		if (msg) ms_free(mc_message_deinit(msg));
		rendpt->rdbuffer.nbytes = rdsize;
	}
}

mc_endpt_udp_t* mc_endpt_udp_start(mc_endpt_udp_t* const endpt, mc_endpt_read_fn_t readfn) {
	endpt->running = 1;
	endpt->readfn = readfn;
	endpt->thread = ms_thread_init(ms_thread_alloc(), (ms_thread_fn_t*)&endpt_udp_reader, endpt);

	return endpt;
}

mc_endpt_udp_t* mc_endpt_udp_stop(mc_endpt_udp_t* const endpt) {
	endpt->running = 0;
	ms_free(ms_thread_deinit(endpt->thread));
	endpt->thread = 0;

	return endpt;
}

mc_message_t* mc_endpt_udp_recv(mc_endpt_udp_t* const endpt) {
	sockaddr_t fromaddr;
	socklen_t addrlen;
	uint32_t bpos;
	uint32_t rdsize;
	mc_message_t* msg;

	rdsize = endpt->rdbuffer.nbytes;
	mn_socket_recvfrom(&endpt->sock, (char*)endpt->rdbuffer.bytes, endpt->rdbuffer.nbytes, (size_t*)&endpt->rdbuffer.nbytes, &fromaddr, &addrlen, &endpt->tmout);

	bpos = 0;
	msg = mc_message_from_buffer(mc_message_alloc(), &endpt->rdbuffer, &bpos);
	endpt->rdbuffer.nbytes = rdsize;

	return msg;
}

int mc_endpt_udp_send(mc_endpt_udp_t* const endpt, sockaddr_t* toaddr, mc_message_t* msg) {
	uint32_t bufsize;
	size_t sent;
	int err;

	bufsize = mc_message_to_buffer(msg, &endpt->wrbuffer);
	err = mn_socket_sendto(&endpt->sock, (const char*)endpt->wrbuffer.bytes, bufsize, &sent, toaddr, (socklen_t)sizeof(struct sockaddr_in), &endpt->tmout);
	return err;
}

/** @} */