#ifndef MC_ENDPT_UDP_H
#define MC_ENDPT_UDP_H

/** 
 * @file
 * @defgroup endpt_udp UDP CoAP Endpoint
 * @{
 */

#include "msys/ms_config.h"
#include "msys/ms_thread.h"
#include "mnet/mn_socket.h"
#include "mcoap/mc_buffer.h"
#include "mcoap/mc_message.h"

typedef int (*mc_endpt_read_fn_t)(sockaddr_t* from, mc_message_t* msg);

typedef struct mc_endpt_udp mc_endpt_udp_t;
struct mc_endpt_udp {
	ms_thread_t* thread;
	mn_socket_t sock;
	mn_timeout_t tmout;
	mc_endpt_read_fn_t readfn;
	mc_buffer_t rdbuffer;
	mc_buffer_t wrbuffer;
	int running;
};

mc_endpt_udp_t* mc_endpt_udp_alloc();
mc_endpt_udp_t* mc_endpt_udp_init(mc_endpt_udp_t* const endpt, uint32_t rdsize, uint32_t wrsize, const char* hostname, unsigned short port);
mc_endpt_udp_t* mc_endpt_set_timeout(mc_endpt_udp_t* const endpt, double seconds);
mc_endpt_udp_t* mc_endpt_udp_deinit(mc_endpt_udp_t* const endpt);
mc_endpt_udp_t* mc_endpt_udp_start(mc_endpt_udp_t* const endpt, mc_endpt_read_fn_t readfn);
mc_endpt_udp_t* mc_endpt_udp_stop(mc_endpt_udp_t* const endpt);
mc_message_t* mc_endpt_udp_recv(mc_endpt_udp_t* const endpt);
int mc_endpt_udp_send(mc_endpt_udp_t* const endpt, sockaddr_t* toaddr, mc_message_t* msg);

/** @} */

#endif
