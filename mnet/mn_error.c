#include "mnet/mn_error.h"

/**
 * @file
 * @ingroup socket
 * @{
 * Error strings
 */
const char *mn_strerror(int err) {
    switch (err) {
        case MN_CLOSED: return "closed";
        case MN_DONE: return "done";
        case MN_TIMEOUT: return "timeout";
        default: return "unknown"; 
    }
}

/** @} */
