#include "msys/ms_log.h"
#include "msys/ms_memory.h"
#include "mcoap/mc_endpt_udp.h"
#include "mcoap/mc_buffer_queue.h"

// @todo need an entry_deinit() function.

/** 
 * Allocate an entry for the queue.
 * @return the entry.
 */
static mc_buffer_queue_entry_t* queue_entry_alloc() {
    return (mc_buffer_queue_entry_t*)calloc(1, sizeof(mc_buffer_queue_entry_t));
}

/**
 * Generate a timeout value between ACK_TIMEOUT and ACK_TIMEOUT * ACK_RANDOM_FACTOR.
 * @return the timeout.
 */
static double mk_timeout() {
	/* Make sure everything's treated as a double. */
	double ack = ACK_TIMEOUT;
	double factor = ACK_RANDOM_FACTOR;

	/* Delta is the interval between ACK_TIMEOUT and ACK_TIMEOUT * ACK_RANDOM_FACTOR. */
	double delta = (ack * factor) - ack;

	/* Generate a random number from 0..1 to multiply delta by. */
	double randfraction = (double)rand();
	randfraction = randfraction / RAND_MAX;

	return ack + (randfraction * delta);
}

/**
 * Initialize the entry fields.
 * @return the initialized entry.
 */
static mc_buffer_queue_entry_t* queue_entry_init(
		mc_buffer_queue_entry_t* entry,
		uint32_t msgid,
		sockaddr_t* dest,
		mc_buffer_t* msg,
		mc_endpt_result_fn_t resultfn,
		mc_buffer_queue_entry_t* prev,
		mc_buffer_queue_entry_t* next) {
    entry->msgid = msgid;
    entry->dest = dest;
    entry->msg = msg;
    entry->resultfn = resultfn;
    entry->prev = prev;
    entry->next = next;

    entry->xmitcounter = 0;

    mn_timeout_init(&entry->timeout, -1.0, mk_timeout());
    mn_timeout_markstart(&entry->timeout);

    return entry;
}

static mc_buffer_queue_entry_t* queue_entry_deinit(mc_buffer_queue_entry_t* entry) {
    entry->msgid = 0;
    ms_free(entry->dest);
    entry->dest = 0;
    ms_free(mc_buffer_deinit(entry->msg));
    entry->msg = 0;
    entry->resultfn = 0;
    entry->prev = 0;
    entry->next = 0;
    entry->xmitcounter = 0;

    mn_timeout_init(&entry->timeout, 0.0, 0.0);

    return entry;
}

/**
 * Allocate a buffer queue.
 * @return the allocated queue.
 */
mc_buffer_queue_t* mc_buffer_queue_alloc() {
    return (mc_buffer_queue_t*)calloc(1, sizeof(mc_buffer_queue_t));
}

/** 
 * Init a queue.
 * @return the queue.
 */
mc_buffer_queue_t* mc_buffer_queue_init(mc_buffer_queue_t* queue) {
    queue->first = 0;
    queue->last = 0;

    return queue;
}

/** 
 * Add to the end of the queue.
 * @return the queue.
 */
mc_buffer_queue_entry_t* mc_buffer_queue_add(mc_buffer_queue_t* queue, uint16_t msgid, sockaddr_t* dest, mc_buffer_t* msg, mc_endpt_result_fn_t resultfn) {

    mc_buffer_queue_entry_t* entry;

    if (queue->first == 0) {
        entry = queue_entry_init(queue_entry_alloc(), msgid, dest, msg, resultfn, 0, 0);
        queue->first = entry;
        queue->last = queue->first;
    }
    else {
        entry = queue_entry_init(queue_entry_alloc(), msgid, dest, msg, resultfn, queue->last, 0);
        queue->last->next = entry;
        queue->last = entry;
    }
    ms_log_debug("add msgid: %d", msgid);

    return entry;
}

/**
 * Count the number of entries in the queue.
 * @return the count.
 */
uint32_t mc_buffer_queue_count(const mc_buffer_queue_t* queue) {
    mc_buffer_queue_entry_t* current = queue->first;
    uint32_t count = 0;

    while (current) {
        count++;
        current = current->next;
    }

    return count;
}

/**
 * Remove an entry from the queue.
 * @return the entry with the prevous and next pointers 0'd.
 */
static mc_buffer_queue_entry_t* remove_entry(mc_buffer_queue_t* queue, mc_buffer_queue_entry_t* current) {
    /* Detatch the prev node and make it point to the next node. */
    /* If this current is at the top then we just have to reset queue->first. */
    if (current->prev) {
        current->prev->next = current->next;
    } 
    else {
        queue->first = current->next;
    }

    /* Detach the next node and make it point to the prev node. */
    /* If current is on the bottom we just have to reset queue->last. */
    if (current->next) {
        current->next->prev = current->prev;
    }
    else {
        queue->last = current->prev;
    }

    /* Clear prev, next pointers so the caller can't use them. */
    current->prev = 0;
    current->next = 0;

    return current;
}

/**
 * Remove the given entry and delete it.
 * Unlike many functions, it actually frees the entry pointer
 * and not just deinit'ing its contents.
 * @return the *next* entry in the list.
 */
mc_buffer_queue_entry_t* mc_buffer_queue_remove_entry(mc_buffer_queue_t* queue, mc_buffer_queue_entry_t* entry) {
    mc_buffer_queue_entry_t* result;
    if (entry == 0) return 0;

    result = entry->next;
    entry = remove_entry(queue, entry);
    ms_free(queue_entry_deinit(entry));

    return result;
}

/**
 * Return the entry with the matching message id.
 * @return the matching entry or 0 if queue is 0 or entry not found.
 */
mc_buffer_queue_entry_t* mc_buffer_queue_get(mc_buffer_queue_t* queue, uint16_t msgid) {
    mc_buffer_queue_entry_t* current;

    if (queue == 0) return 0;

    current = queue->first;
    while (current) {
        if (current->msgid == msgid) {
            return current;
        }
        else {
            current = current->next;
        }
    }
    return 0;
}

/**
 * Remove the next entry with the given flow_id.
 * @return the queued msg_id or UINT32_MAX if not found.
 */
uint32_t mc_buffer_queue_remove(mc_buffer_queue_t* queue, uint16_t msgid) {
    mc_buffer_queue_entry_t* entry = mc_buffer_queue_get(queue, msgid);
    if (entry == 0) return UINT32_MAX;

    mc_buffer_queue_remove_entry(queue, entry);
    return msgid;
}

/**
 * Free the contents of a queue.
 * @return the queue pointer
 */
mc_buffer_queue_t* mc_buffer_queue_free(mc_buffer_queue_t* queue) {
    while (queue->first) {
        mc_buffer_queue_remove_entry(queue, queue->first);
    }
    return queue;
}

/**
 * Return the next entry (including this one) that has a timeout.
 * If this routine returns a timed out entry, to find the next onec
 * call the function again with entry->next;
 * @return pointer to a timed out entry, 0 if none.
 */
mc_buffer_queue_entry_t* mc_buffer_queue_next_timeout(mc_buffer_queue_entry_t* entry) {
    while (entry && (mn_timeout_get(&entry->timeout) != 0.0)) {
    	entry = entry->next;
    }

    return entry;
}

/**
 * Scan the pending queue for timeouts, but do not modify the queue.
 * @return the true if one found, false otherwise.
 */
int mc_buffer_queue_has_timeout(const mc_buffer_queue_t* queue) {
    return (mc_buffer_queue_next_timeout(queue->first) != 0);
}

/**
 * Scan the pending queue for timeouts and remove the first one found.
 * @return the first timed out entry or 0 if none found.
 */
mc_buffer_queue_entry_t* mc_buffer_queue_timeout(mc_buffer_queue_t* queue) {
    mc_buffer_queue_entry_t* current = queue->first;
    mc_buffer_queue_entry_t* result = 0;

    while (current) {
        if (mn_timeout_get(&current->timeout) == 0.0) {
            result = remove_entry(queue, current);
            current = 0;
        }
        else {
            current = current->next;
        }
    }

    return result;
}
