#include <assert.h>

#include "msys/ms_thread.h"
#include "msys/ms_memory.h"

/**
 * @file
 * @ingroup thread
 * @{
 * This file implements threading using windows calls.
 */

typedef struct thread_win thread_win_t;
struct thread_win {
    us_thread_fn_t* thread_fn;
    void* rdata;
    HANDLE handle;
};

/** Alloc a local thread structure. */
ms_thread_t* ms_thread_alloc() {
    return (ms_thread_t*)ms_calloc(1, thread_win_t);
}

static unsigned int __stdcall thread_main_fn(void* arg) {
    thread_win_t* thread  = (thread_win_t*)arg;

    /*  Run the thread routine. */
    thread->thread_fn(thread->rdata);
    return 0;
}

ms_thread_t* ms_thread_init(ms_thread_t* thread, ms_thread_fn_t* thread_fn, void* rdata) {
    thread_win_t* mythread = (thread_win_t*)thread;
    
    mythread->routine = routine;
    mythread->rdata = rdata;
    mythread->handle = (HANDLE)_beginthreadex (NULL, 0, thread_main_fn, (void*)mythread, 0 , NULL);
    assert(mythread->handle != NULL);
    
    return thread;
}

ms_thread_t* ms_thread_deinit(ms_thread_t* thread) {
    DWORD rc;
    BOOL brc;
    thread_win_t* mythread;
    
    mythread = (thread_win_t*)thread;

    rc = WaitForSingleObject (mythread->handle, INFINITE);
    assert(rc != WAIT_FAILED);
    brc = CloseHandle (mythread->handle);
    assert(brc != 0);
    
    return thread;
}

/** @} */
