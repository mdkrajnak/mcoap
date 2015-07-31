#ifndef MN_ERROR_H
#define MN_ERROR_H

/** 
 * @file
 * @ingroup socket
 * @{
 */

#include <stdio.h>

/* Error codes */
enum {
    MN_DONE = 0,        /* operation completed successfully */
    MN_TIMEOUT = -1,    /* operation timed out */
    MN_CLOSED = -2,     /* the connection has been closed */
    MN_UNKNOWN = -3
};

const char *mn_error(int err);

/** @} */

#endif

