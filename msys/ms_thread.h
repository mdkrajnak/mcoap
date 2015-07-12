#ifndef MS_THREAD_H
#define MS_THREAD_H

/**
 * @file
 * @defgroup thread Simple cross platform thread furntions.
 * @{
 * Simple cross platform thread furntions.
 */
 
#include "msys/ms_config.h"

typedef void* ms_thread_t;
typedef void (*ms_thread_fn_t)(void*);

ms_thread_t* ms_thread_alloc();
ms_thread_t* ms_thread_init(ms_thread_t* thread, ms_thread_fn_t* thread_fn, void* arg);
ms_thread_t* ms_thread_deinit(ms_thread_t* thread);

/** @} */

#endif

