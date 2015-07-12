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
#include "mcoap/mc_message.h"

typedef int (*mc_endpt_read_fn_t)(mc_message_t* msg);

typedef struct mc_endpt_udp mc_endpt_udp_t;
struct mc_endpt_udp {
	ms_thread_t* thread;
	mn_socket_t sock;
	mc_endpt_read_fn_t readfn;
	size_t bufsize;
	int running;
};

mc_endpt_udp_t* mc_endpt_udp_alloc();
mc_endpt_udp_t* mc_endpt_udp_init(mc_endpt_udp_t* endpt, size_t bufsize, const char* hostname, unsigned short port);
mc_endpt_udp_t* mc_endpt_udp_deinit(mc_endpt_udp_t* endpt);
int mc_endpt_udp_start(mc_endpt_udp_t* endpt, mc_endpt_read_fn_t readfn);
int mc_endpt_udp_stop(mc_endpt_udp_t* endpt);
int mc_endpt_udp_write(mc_endpt_udp_t* endpt, mc_message_t* msg);

/** @} */

#endif
