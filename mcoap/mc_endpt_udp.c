/**
 * @file
 * @ingroup endpt_udp
 * @{
 */

#include "msys/ms_config.h"
#include "msys/ms_memory.h"
#include "msys/ms_log.h"
#include "mnet/mn_timeout.h"
#include "mnet/mn_sockaddr.h"
#include "mcoap/mc_endpt_udp.h"
#include "mcoap/mc_code.h"
#include "mcoap/mc_token.h"
#include "mcoap/mc_uri.h"

#include <math.h>

#define DEFAULT_ENDPT_TIMEOUT 0.05

mc_endpt_udp_t* mc_endpt_udp_alloc() {
    return ms_calloc(1, mc_endpt_udp_t);
}

/**
 * RFC7252 4.4 recommends choosing a random initial value.
 */
static uint16_t random_id() {
    unsigned int seed = (unsigned int)round(mn_gettime()*1000.0);
    srand(seed);
    return (uint16_t)rand();
}

mc_endpt_udp_t* mc_endpt_udp_init(mc_endpt_udp_t* const endpt, uint32_t rdsize, uint32_t wrsize, const char* hostname, unsigned short port) {
    sockaddr_t addr;
    int err;
    endpt->readfn = 0;
    endpt->thread = 0;
    endpt->running = 0;
    endpt->nextid = random_id();
    if (endpt->nextid == 0) {
        endpt->nextid = 1;
    }

    /* @todo consider restricting the maxmimum buffer sizes (e.g. less then 64k). */
    mc_buffer_init(&endpt->rdbuffer, rdsize, ms_calloc(rdsize, uint8_t));
    mc_buffer_init(&endpt->wrbuffer, wrsize, ms_calloc(wrsize, uint8_t));

    mc_buffer_queue_init(&endpt->confirmq);

    mn_timeout_init(&endpt->tmout, DEFAULT_ENDPT_TIMEOUT, -1.0);
    if (hostname == 0) {
        ms_log_debug("Hostname is null");
        return 0;
    }

    if (!mn_sockaddr_inet_init(&addr, hostname, port)) {
        ms_log_debug("Failed to initialize address %s:%d", hostname, port);
        return 0;
    }

    err = mn_socket_create(&endpt->sock, AF_INET, SOCK_DGRAM, 0);
    if ( err != MN_DONE) {
        ms_log_debug("Failed to create socket, error: %d, %s.", err, mn_strerror(err));
        return 0;
    }

    err = mn_socket_bind(&endpt->sock, &addr, sizeof(sockaddr_t));
    if (err != MN_DONE) {
        ms_log_debug("Error %s (%d) binding socket on %s:%d", mn_strerror(err), err, hostname, port);
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

/** Primary worker function for executing a read/dispatch loop. */
static void endpt_udp_loop(mc_endpt_udp_t* endpt) {
    mn_timeout_t tout;

    /* Initialize timeout. */
    /* @todo should initializing the timeout be part of mc_endpt_udp_recv()? */
    /* @todo parameterize the timeout values. */
    mn_timeout_init(&tout, 0.2, -0.2);

    while (endpt->running) {
        /* Allocate and read a message. */
        mc_message_t* msg = mc_endpt_udp_recv(endpt);

        if (msg) {
            /* There's no locking around setting the running flag. */
            /* This should be fine as longer as the using program does not set running outside this thread. */
            endpt->running = endpt->readfn(msg);
            ms_free(mc_message_deinit(msg));
        }

        mc_endpt_udp_check_queues(endpt);
    }
}

static void endpt_udp_reader(void* data) {
    mc_endpt_udp_t* rendpt = (mc_endpt_udp_t*)data;
    endpt_udp_loop(rendpt);
}

/**
 * Run the read dispatch loop until either the readfn returns false or
 * the calling program sets endpt->running to 0.
 */
void mc_endpt_udp_loop(mc_endpt_udp_t* endpt, mc_endpt_read_fn_t readfn) {
    endpt->running = 1;
    endpt->readfn = readfn;
    endpt_udp_loop(endpt);
}

uint16_t mc_endpt_udp_nextid(mc_endpt_udp_t* endpt) {
    uint16_t result = endpt->nextid;
    endpt->nextid++;
    if (endpt->nextid == 0) {
        endpt->nextid = 1;
    }

    return result;
}

/**
 * Run the read dispatch loop in a background thread.
 */
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

static void call_result_fn(mc_endpt_udp_t* endpt, mc_endpt_result_fn_t resultfn, uint16_t msgid, int err) {
    if (resultfn > (mc_endpt_result_fn_t)1) {
        (*resultfn)((mc_endpt_id_t)endpt, msgid, err);
    }
}

mc_message_t* mc_endpt_udp_recv(mc_endpt_udp_t* const endpt) {
    sockaddr_t fromaddr;
    socklen_t addrlen;
    uint32_t bpos;
    uint32_t rdsize;
    mc_message_t* msg;
    int err;

    addrlen = sizeof fromaddr;
    rdsize = endpt->rdbuffer.nbytes;
    mn_timeout_markstart(&endpt->tmout);
    err = mn_socket_recvfrom(&endpt->sock, (char*)endpt->rdbuffer.bytes, endpt->rdbuffer.nbytes, (size_t*)&endpt->rdbuffer.nbytes, &fromaddr, &addrlen, &endpt->tmout);

    if (err != MN_DONE) {
        msg = 0;
        ms_log_debug("Receive error: %d, %s", err, mn_strerror(err));
    }
    else {
        bpos = 0;
        ms_log_debug("Received response with %d bytes", endpt->rdbuffer.nbytes);
        /* ms_log_bytes(ms_debug, endpt->rdbuffer.nbytes, endpt->rdbuffer.bytes); */

        msg = mc_message_from_buffer(mc_message_alloc(), &endpt->rdbuffer, &bpos);
        msg->from = mn_sockaddr_copy(&fromaddr);
    }

    /** Reset the read buffer size to it original value. */
    endpt->rdbuffer.nbytes = rdsize;

    if (mc_message_is_ack(msg)) {
        uint16_t msgid = mc_message_get_message_id(msg);
        mc_buffer_queue_entry_t* entry = mc_buffer_queue_get(&endpt->confirmq, msgid);

        /* Note that err should be MN_DONE at this point. */
        call_result_fn(endpt, entry->resultfn, entry->msgid, err);
        mc_buffer_queue_remove_entry(&endpt->confirmq, entry);
    }

    return msg;
}

static int send_entry_buffer(mc_endpt_udp_t* const endpt, mc_buffer_queue_entry_t* entry) {
    size_t sent;
    int err;
    socklen_t tolen = (socklen_t)sizeof(struct sockaddr_in);

    if (entry->xmitcounter >= MAX_RETRANSMIT) {
        err = MN_TIMEOUT;
    }
    else {
        mn_timeout_markstart(&endpt->tmout);
        err = mn_socket_sendto(&endpt->sock, (char*)entry->msg->bytes, entry->msg->nbytes, &sent, entry->dest, tolen, &endpt->tmout);

        entry->xmitcounter++;
    }
    return err;
}

/**
 * Message has already been serialized into the output buffer.
 */
static int send_endpt_buffer(mc_endpt_udp_t* const endpt, uint32_t nbytes, sockaddr_t* toaddr) {
    size_t sent;
    socklen_t tolen = (socklen_t)sizeof(struct sockaddr_in);

    mn_timeout_markstart(&endpt->tmout);
    return mn_socket_sendto(&endpt->sock, (const char*)endpt->wrbuffer.bytes, nbytes, &sent, toaddr, tolen, &endpt->tmout);
}

/**
 * Only used the first time we send a confirmable msg as this loads it into the confirm queue.
 * Retransmits are sent by send_entry_buffer() when we check the the buffer queues.
 * 
 */
static int send_con_msg(mc_endpt_udp_t* const endpt, uint32_t nbytes, sockaddr_t* toaddr, mc_message_t* msg, mc_endpt_result_fn_t resultfn) {
    mc_buffer_queue_entry_t* entry = mc_buffer_queue_add(
        &endpt->confirmq,
        mc_message_get_message_id(msg),
        mn_sockaddr_copy(toaddr),
        mc_buffer_copy(&endpt->wrbuffer, 0, nbytes),
        resultfn);

    int err = send_endpt_buffer(endpt, nbytes, toaddr);
    if (err == MN_DONE) {
        entry->xmitcounter++;
    }
    else {
        call_result_fn(endpt, resultfn, mc_message_get_message_id(msg), err);
        mc_buffer_queue_remove_entry(&endpt->confirmq, entry);
    }
    return err;
}

int mc_endpt_udp_send(mc_endpt_udp_t* const endpt, sockaddr_t* toaddr, mc_message_t* msg, mc_endpt_result_fn_t resultfn) {
    uint32_t nbytes;
    int err;

    /* Serialize the mesage into the endpt's write buffer. */
    nbytes = mc_message_to_buffer(msg, &endpt->wrbuffer);

    if (mc_message_is_confirmable(msg)) {
        err = send_con_msg(endpt, nbytes, toaddr, msg, resultfn);
    }
    else {
        err = send_endpt_buffer(endpt, nbytes, toaddr);
    }

    return err;
}

/**
 * Iterate over the confirmation queue entries, if timeout retransmit
 * or notify client of error if too many tries.
 *
 * NOTE! The entry is *NOT* removed until after the result function is called
 * Consider adding an accessor that the callee can use to inspect the failed message
 * while executing the result function.
 */
mc_endpt_udp_t* mc_endpt_udp_check_queues(mc_endpt_udp_t* const endpt) {
    int err;
    mc_buffer_queue_entry_t* current;

    current = endpt->confirmq.first;
    while (current) {
        if (mn_timeout_get(&current->timeout) == 0.0) {

            /* Double the timeout we'll wait for the confirm. */
            /* Implements the exponential back off algorithm. */
            mn_timeout_init(&current->timeout, -1.0, current->timeout.total * 2.0);
            mn_timeout_markstart(&current->timeout);

            err = send_entry_buffer(endpt, current);
            if (err != MN_DONE) {
                ms_log_debug("Error: %d, resending confirmable: %d, xmit: %d", err, current->msgid, current->xmitcounter);
                call_result_fn(endpt, current->resultfn, current->msgid, err);
                current = mc_buffer_queue_remove_entry(&endpt->confirmq, current);
            }
            else {
                current = current->next;
            }
        }
        else {
            current = current->next;
        }
    }

    return endpt;
}

/**
 * Send a "ack only" message, e.g. without a piggy backed response.
 * @return send error code.
 */
int mc_endpt_udp_ack(mc_endpt_udp_t*const endpt, sockaddr_t*const addr, mc_buffer_t* token, uint16_t msgid) {
    mc_message_t msg;
    int err;

    // @todo what should the token setting be for an ack message?
    mc_message_init(&msg, 1, MC_ACK, 0, msgid, token, 0, 0);

    err = mc_endpt_udp_send(endpt, addr, &msg, 0);
    if (err != MN_DONE) {
        ms_log_debug("Error sending message: %d, %s", err, mn_strerror(err));
    }

    mc_message_deinit(&msg);
    return err;
}

/**
 * Create options from URI and to address.
 * If there are extra options merge them into the list.
 */
static  mc_options_list_t* mk_options(char* const uri, sockaddr_t* const addr, mc_options_list_t* extra) {
    mc_options_list_t* list = mc_uri_to_options(mc_options_list_alloc(), addr, uri);
    if (extra != 0) {
        mc_options_list_t* merged = mc_options_list_merge(list, extra);
        ms_free(mc_options_list_deinit(list));
        list = merged;
    }
    return list;
}

static uint16_t mk_message(mc_message_t* msg, mc_endpt_udp_t* const endpt, uint8_t code, mc_endpt_result_fn_t resultfn, mc_options_list_t* list, mc_buffer_t* payload) {
    uint16_t msgid = mc_endpt_udp_nextid(endpt);
    mc_buffer_t* token = mc_token_create2(msgid);

    if (resultfn != 0) {
        mc_message_con_init(msg, code, msgid, token, list, payload);
    }
    else {
        mc_message_non_init(msg, code, msgid, token, list, payload);
    }

    return msgid;
}

/**
 * Generic message sending function to suppport implementation of
 * mc_endpt_udp_get, put, post, and delete.
 */
static uint16_t send_msg(mc_endpt_udp_t* const endpt, sockaddr_t* const addr, mc_endpt_result_fn_t resultfn, uint8_t code, char* const uri, mc_options_list_t* extra, mc_buffer_t* payload) {
    mc_message_t msg;
    mc_options_list_t* list;
    uint16_t msgid;
    int err;

    list = mk_options(uri, addr, extra);
    msgid = mk_message(&msg, endpt, code, resultfn, list, payload);

    err = mc_endpt_udp_send(endpt, addr, &msg, resultfn);
    if (err != MN_DONE) {
        ms_log_debug("Error sending message: %d, %s", err, mn_strerror(err));
        msgid = 0;
    }

    mc_message_deinit(&msg);
    return msgid;
}

/**
 * Get the uri specified, include extra options if any.
 * @return the message id.
 */
uint16_t mc_endpt_udp_delete(mc_endpt_udp_t* const endpt, sockaddr_t* const addr, mc_endpt_result_fn_t resultfn, char* const uri, mc_options_list_t* extra) {
    return send_msg(endpt, addr, resultfn, MC_DELETE, uri, extra, 0);
}

/**
 * Get the uri specified, include extra options if any.
 * @return the message id.
 */
uint16_t mc_endpt_udp_get(mc_endpt_udp_t* const endpt, sockaddr_t* const addr, mc_endpt_result_fn_t resultfn, char* const uri, mc_options_list_t* extra) {
    return send_msg(endpt, addr, resultfn, MC_GET, uri, extra, 0);
}

/**
 * Post the uri specified, include extra options if any.
 * @return the message id.
 */
uint16_t mc_endpt_udp_post(mc_endpt_udp_t* const endpt, sockaddr_t* const addr, mc_endpt_result_fn_t resultfn, char* const uri, mc_options_list_t* extra, mc_buffer_t* payload) {
    return send_msg(endpt, addr, resultfn, MC_POST, uri, extra, payload);
}

/**
 * Put the uri specified, include extra options if any.
 * @return the message id.
 */
uint16_t mc_endpt_udp_put(mc_endpt_udp_t* const endpt, sockaddr_t* const addr, mc_endpt_result_fn_t resultfn, char* const uri, mc_options_list_t* extra, mc_buffer_t* payload) {
    return send_msg(endpt, addr, resultfn, MC_PUT, uri, extra, payload);
}

/** @} */
