#include <assert.h>
#include <pthread.h>

#include "msys/ms_config.h"
#include "msys/ms_memory.h"
#include "msys/ms_mutex.h"

/**
 * @file
 * @defgroup mutex 
 * @{
 * Simple cross platform mutex function implementations using posix calls.
 */

/** Implementation private mutex structure. */
typedef struct mutex_posix mutex_posix_t;
struct mutex_posix {
    pthread_mutex_t mutex; /**< mutex. */
};

/**
 * Allocate a mutex.
 * @return the allocated mutex.
 */
ms_mutex_t* ms_mutex_alloc() {
    return (ms_mutex_t*)ms_calloc(1, mutex_posix_t);
}

/**
 * Initialize a mutex.
 * @return the initialized mutex.
 */
ms_mutex_t* ms_mutex_init(ms_mutex_t* mutex) {
    int rc;
    mutex_posix_t* mymutex;
    
    mymutex = (mutex_posix_t*)mutex;

    rc = pthread_mutex_init (&mymutex->mutex, NULL);
    assert(rc == 0);
    return mutex;
}

/**
 * Deinitialize a mutex.
 */
ms_mutex_t* ms_mutex_deinit(ms_mutex_t* mutex) {
    int rc;
    mutex_posix_t* mymutex;
    
    mymutex = (mutex_posix_t*)mutex;

    rc = pthread_mutex_destroy (&mymutex->mutex);
    assert(rc == 0);
    return mutex;
}

/**
 * Lock a mutex.
 * @return 0 on failure.
 */
int ms_mutex_lock(ms_mutex_t* mutex) {
    int rc;
    mutex_posix_t* mymutex;
    
    mymutex = (mutex_posix_t*)mutex;

    rc = pthread_mutex_lock (&mymutex->mutex);
    return rc == 0;
}

/**
 * Unlock a mutex.
 * @return 0 on failure.
 */
int ms_mutex_unlock(ms_mutex_t* mutex) {
    int rc;
    mutex_posix_t* mymutex;
    
    mymutex = (mutex_posix_t*)mutex;

    rc = pthread_mutex_unlock (&mymutex->mutex);
    return rc == 0;
}

/** @} */


