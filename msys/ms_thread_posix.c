#include <signal.h>
#include <assert.h>
#include <pthread.h>

#include "msys/ms_thread.h"
#include "msys/ms_memory.h"

/**
 * @file
 * @ingroup thread
 * @{
 * This file implements threading using posix calls.
 */

typedef struct thread_posix thread_posix_t;
struct thread_posix {
    ms_thread_fn_t* thread_fn;
    void* rdata;
    pthread_t handle;
};


/** Alloc a local thread structure. */
ms_thread_t* ms_thread_alloc() {
    return (ms_thread_t*)ms_calloc(1, thread_posix_t);
}

static void* thread_main_fn(void* arg) {
    thread_posix_t* thread  = (thread_posix_t*)arg;

    /*  Run the thread routine. */
    (*thread->thread_fn)(thread->rdata);
    return NULL;
}

ms_thread_t* ms_thread_init (ms_thread_t* thread, ms_thread_fn_t* thread_fn, void* rdata) {
    int rc;
    sigset_t new_sigmask;
    sigset_t old_sigmask;
    thread_posix_t* mythread;
    
    mythread = (thread_posix_t*)thread;
 
    /*  The library does not use any signals internally. */
    rc = sigfillset(&new_sigmask);
    assert(rc == 0);
    rc = pthread_sigmask(SIG_BLOCK, &new_sigmask, &old_sigmask);
    assert(rc == 0);

    mythread->thread_fn = thread_fn;
    mythread->rdata = rdata;
    rc = pthread_create(&mythread->handle, NULL, thread_main_fn, (void*)mythread);
    assert(rc == 0);

    /*  Restore signal set to what it was before. */
    rc = pthread_sigmask(SIG_SETMASK, &old_sigmask, NULL);
    assert(rc == 0);
    
    return thread;
}

ms_thread_t* ms_thread_deinit(ms_thread_t* thread) {
    int rc;
    thread_posix_t* mythread;
    
    mythread = (thread_posix_t*)thread;
    rc = pthread_join(mythread->handle, NULL);
    assert(rc == 0);
    
    return thread;
}

/** @} */
