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
#include "mcoap/mc_buffer_queue.h"

/* @todo consider creating a struct and init-with-defaults function for this information. */
/* Default transmission parameters. */
#define ACK_TIMEOUT       2  	/**< seconds. */
#define ACK_RANDOM_FACTOR 1.5	/**< multiplier. */
#define MAX_RETRANSMIT    4		/**< count. */
#define NSTART            1		/**< count. */
#define LEISURE           5		/**< seconds. */
#define PROBING_RATE      1		/**< bytes/second. */

#define MAX_TRANSMIT_SPAN	45	/* ACK_TIMEOUT * ((2 ** MAX_RETRANSMIT) - 1) * ACK_RANDOM_FACTOR */
#define MAX_LATENCY			100
#define PROCESSING_DELAY	ACK_TIMEOUT
#define EXCHANGE_LIFETIME   247	/* MAX_TRANSMIT_SPAN + (2 * MAX_LATENCY) + PROCESSING_DELAY */

typedef int (*mc_endpt_read_fn_t)(sockaddr_t* from, mc_message_t* msg);

typedef struct mc_endpt_udp mc_endpt_udp_t;
struct mc_endpt_udp {
	ms_thread_t* thread;
	mn_socket_t sock;
	mn_timeout_t tmout;
	mc_endpt_read_fn_t readfn;
	mc_buffer_t rdbuffer;
	mc_buffer_t wrbuffer;
	mc_buffer_queue_t confirmq;
	int running;
	uint16_t nextid;
};

mc_endpt_udp_t* mc_endpt_udp_alloc();
mc_endpt_udp_t* mc_endpt_udp_init(mc_endpt_udp_t* const endpt, uint32_t rdsize, uint32_t wrsize, const char* hostname, unsigned short port);
mc_endpt_udp_t* mc_endpt_set_timeout(mc_endpt_udp_t* const endpt, double seconds);
mc_endpt_udp_t* mc_endpt_udp_deinit(mc_endpt_udp_t* const endpt);
mc_endpt_udp_t* mc_endpt_udp_start(mc_endpt_udp_t* const endpt, mc_endpt_read_fn_t readfn);
mc_endpt_udp_t* mc_endpt_udp_stop(mc_endpt_udp_t* const endpt);
mc_message_t* mc_endpt_udp_recv(mc_endpt_udp_t* const endpt);
int mc_endpt_udp_send(mc_endpt_udp_t* const endpt, sockaddr_t* toaddr, mc_message_t* msg, mc_endpt_result_fn_t resultfn);
mc_endpt_udp_t* mc_endpt_udp_check_queues(mc_endpt_udp_t* const endpt);
int mc_endpt_udp_ack(mc_endpt_udp_t *const endpt, sockaddr_t *const addr, mc_buffer_t *token, uint16_t msgid);
uint16_t mc_endpt_udp_delete(mc_endpt_udp_t* const endpt, sockaddr_t* const addr, mc_endpt_result_fn_t resultfn,
						     char* const uri, mc_options_list_t* extra);
uint16_t mc_endpt_udp_get(mc_endpt_udp_t* const endpt, sockaddr_t* const addr, mc_endpt_result_fn_t resultfn,
                          char* const uri, mc_options_list_t* extra);
uint16_t mc_endpt_udp_post(mc_endpt_udp_t* const endpt, sockaddr_t* const addr, mc_endpt_result_fn_t resultfn,
						   char* const uri, mc_options_list_t* extra, mc_buffer_t* payload);
uint16_t mc_endpt_udp_put(mc_endpt_udp_t* const endpt, sockaddr_t* const addr, mc_endpt_result_fn_t resultfn,
						  char* const uri, mc_options_list_t* extra, mc_buffer_t* payload);

/** @} */

#endif
