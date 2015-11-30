#include <assert.h>

#include "msys/ms_config.h"
#include "msys/ms_memory.h"
#include "msys/ms_mutex.h"

/**
 * @file
 * @defgroup mutex 
 * @{
 * Simple cross platform mutex function implementations for windows.
 */

typedef struct mutex_win mutex_win_t;
struct mutex_win {
    CRITICAL_SECTION mutex;
};

/**  Allocate a mutex. */
ms_mutex_t* ms_mutex_alloc() {
    return (ms_mutex_t*)ms_calloc(1, mutex_win_t);
}

ms_mutex_t* ms_mutex_init(ms_mutex_t* mutex) {
    mutex_win_t* mymutex = (mutex_win_t*)mutex;
    
    InitializeCriticalSection(&mymutex->mutex);
    return mutex;
}

ms_mutex_t* ms_mutex_term(ms_mutex_t* mutex) {
    mutex_win_t* mymutex = (mutex_win_t*)mutex;
    
    DeleteCriticalSection(&mymutex->mutex);
    return mutex;
}

int ms_mutex_lock(ms_mutex_t* mutex) {
    mutex_win_t* mymutex = (mutex_win_t*)mutex;
    
    EnterCriticalSection(&mymutex->mutex);
    return 1;
}

int ms_mutex_unlock(ms_mutex_t* mutex) {
    mutex_win_t* mymutex = (mutex_win_t*)mutex;
    
    LeaveCriticalSection(&mymutex->mutex);
    return 1;
}

/** @} */

