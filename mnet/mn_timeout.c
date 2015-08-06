#include <stdio.h>

#include "mnet/mn_timeout.h"

/**
 * @file
 * @ingroup socket
 * @{
 */

#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#include <sys/time.h>
#endif

/* min and max macros */
#ifndef MIN
#define MIN(x, y) ((x) < (y) ? x : y)
#endif
#ifndef MAX
#define MAX(x, y) ((x) > (y) ? x : y)
#endif

/**
 * Initialize structure
 * Set block < 0 and total < 0 for an unlimited timeout.
 * Set block > 0 and total < 0 for for a fixed absolute timeout time.
 * Set block < 0 and total > 0 for a timeout relative to start, call timeout_markstart() to reset.
 * If both are > 0, it returns the shortest of either the block value or the computed remaining time.
 */
void mn_timeout_init(mn_timeout_t* tout, double block, double total) {
    tout->block = block;
    tout->total = total;
    tout->start = 0;
}

/**
 * Determines how much time we have left for the next system call,
 * if the previous call was successful 
 * @return the number of ms left or -1 if there is no time limit
 */
double mn_timeout_get(mn_timeout_t* tout) {
    if (tout->block < 0.0 && tout->total < 0.0) {
        return -1;
    } 
    else if (tout->block < 0.0) {
        double delta = tout->total - mn_gettime() + tout->start;
        return MAX(delta, 0.0);
    } 
    else if (tout->total < 0.0) {
        return tout->block;
    } 
    else {
        double delta = tout->total - mn_gettime() + tout->start;
        return MIN(tout->block, MAX(delta, 0.0));
    }
}

/**
 * Returns time since start of operation
 * @return start field of structure
 */
double mn_timeout_getstart(mn_timeout_t* tout) {
    return tout->start;
}

/**
 * Determines how much time we have left for the next system call,
 * if the previous call was a failure
 * @return the number of sec left or -1 if there is no time limit
 */
double mn_timeout_getretry(mn_timeout_t* tout) {
    if (tout->block < 0.0 && tout->total < 0.0) {
        return -1;
    } 
    else if (tout->block < 0.0) {
        double t = tout->total - mn_gettime() + tout->start;
        return MAX(t, 0.0);
    } 
    else if (tout->total < 0.0) {
        double t = tout->block - mn_gettime() + tout->start;
        return MAX(t, 0.0);
    } 
    else {
        double t = tout->total - mn_gettime() + tout->start;
        return MIN(tout->block, MAX(t, 0.0));
    }
}

/**
 * Marks the operation start time in structure.
 */
mn_timeout_t* mn_timeout_markstart(mn_timeout_t* tout) {
    tout->start = mn_gettime();
    return tout;
}

/**
 * Gets time in s, relative to January 1, 1970 (UTC).
 * @return time in seconds.
 */
#ifdef _WIN32
double mn_gettime(void) {
    FILETIME ft;
    double seconds;
    GetSystemTimeAsFileTime(&ft);

    /* Windows file time (time since January 1, 1601 (UTC)) */
    seconds  = ft.dwLowDateTime/1.0e7 + ft.dwHighDateTime*(4294967296.0/1.0e7);

    /* convert to Unix Epoch time (time since January 1, 1970 (UTC)) */
    return (seconds - 11644473600.0);
}
#else
double mn_gettime(void) {
    struct timeval v;
    gettimeofday(&v, (struct timezone *) NULL);

    /* Unix Epoch time (time since January 1, 1970 (UTC)) */
    return v.tv_sec + v.tv_usec/1.0e6;
}
#endif

/**
 * Sleep for n seconds.
 */
int mn_sleep(double n) {
#ifdef _WIN32
    Sleep((int)(n*1000));
#else
    struct timespec t, r;
    t.tv_sec = (int) n;
    n -= t.tv_sec;
    t.tv_nsec = (int) (n * 1000000000);
    if (t.tv_nsec >= 1000000000) t.tv_nsec = 999999999;
    while (nanosleep(&t, &r) != 0) {
        t.tv_sec = r.tv_sec;
        t.tv_nsec = r.tv_nsec;
    }
#endif
    return 0;
}

/** @} */
