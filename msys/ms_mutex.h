#ifndef MS_MUTEX_H
#define MS_MUTEX_H

/**
 * @file
 * @defgroup mutex Simple cross platform mutex functions.
 * @{
 * Simple cross platform mutex function definitions.
 */
 
typedef void* ms_mutex_t;

/**  Allocate a mutex. */
ms_mutex_t* ms_mutex_alloc();

/**  Initialize a mutex. */
ms_mutex_t* ms_mutex_init(ms_mutex_t* mutex);

/** Deinitialize. */
ms_mutex_t* ms_mutex_deinit(ms_mutex_t* mutex);

/**  Lock a mutex. */
int ms_mutex_lock(ms_mutex_t* mutex);

/**  Unlock a mutex. */
int ms_mutex_unlock (ms_mutex_t* mutex);

/** @} */

#endif

