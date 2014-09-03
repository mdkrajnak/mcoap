#ifndef MN_TIMEOUT_H
#define MN_TIMEOUT_H

/**
 * @file
 * @ingroup socket
 * @{
 */

/* Timeout functions. */


/* Timeout structure */
struct mn_timeout {
    double block;          /* maximum time for blocking calls */
    double total;          /* total number of miliseconds for operation */
    double start;          /* time of start of operation */
};

typedef struct mn_timeout mn_timeout_t;


/* Timeout methods. */
void mn_timeout_init(mn_timeout_t* tout, double block, double total);
double mn_timeout_get(mn_timeout_t* tout);
double mn_timeout_getretry(mn_timeout_t* tout);
mn_timeout_t* mn_timeout_markstart(mn_timeout_t* tout);
double mn_timeout_getstart(mn_timeout_t* tout);

/* Utility functions. */
double mn_gettime();
int mn_sleep(double millisecs);

#define mn_timeout_iszero(tout)   ((tout)->block == 0.0)

/** @} */

#endif
