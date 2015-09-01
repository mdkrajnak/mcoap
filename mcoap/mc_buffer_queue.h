#ifndef MC_BUFFER_QUEUE
#define MC_BUFFER_QUEUE

#include "msys/ms_config.h"
#include "mnet/mn_timeout.h"
#include "mcoap/mc_buffer.h"

#define MC_XMIT_TIMEOUT        -1
#define MC_XMIT_ACK_RECEIVED    0

/* And endpt id is the pointer to the endpt that sent the message. */
/* Its encoded this way so the buffer queue can be defined independently of the endpt. */
typedef void* mc_endpt_id_t;

typedef int (*mc_endpt_result_fn_t)(mc_endpt_id_t endpt, uint16_t msgid, int status);

/** Define a queue for confirmable messages for managing retransmits. */
typedef struct mc_buffer_queue_entry mc_buffer_queue_entry_t;
struct mc_buffer_queue_entry {
    uint16_t msgid;
    uint16_t xmitcounter;
    sockaddr_t* dest;
    mn_timeout_t timeout;
    mc_endpt_result_fn_t* resultfn;
    mc_buffer_t* msg;
    mc_buffer_queue_entry_t* prev;
    mc_buffer_queue_entry_t* next;
};


typedef struct mc_buffer_queue mc_buffer_queue_t;
struct mc_buffer_queue {
	mc_buffer_queue_entry_t* first;
	mc_buffer_queue_entry_t* last;
};

mc_buffer_queue_t* mc_buffer_queue_alloc();
mc_buffer_queue_t* mc_buffer_queue_init(mc_buffer_queue_t* queue);
mc_buffer_queue_entry_t* mc_buffer_queue_add(mc_buffer_queue_t* queue, uint16_t msgid, sockaddr_t* dest, mc_buffer_t* msg, mc_endpt_result_fn_t* resultfn);
uint32_t mc_buffer_queue_count(const mc_buffer_queue_t* queue);
mc_buffer_queue_entry_t* mc_buffer_queue_remove_entry(mc_buffer_queue_t* queue, mc_buffer_queue_entry_t* entry);
uint32_t mc_buffer_queue_remove(mc_buffer_queue_t* queue, uint16_t msgid);
mc_buffer_queue_t* mc_buffer_queue_deinit(mc_buffer_queue_t* queue);
mc_buffer_queue_entry_t* mc_buffer_queue_next_timeout(mc_buffer_queue_entry_t* entry);
int mc_buffer_queue_has_timeout(const mc_buffer_queue_t* queue);
mc_buffer_queue_entry_t* mc_buffer_queue_timeout(mc_buffer_queue_t* queue);

#endif
