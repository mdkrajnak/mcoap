#ifndef MS_LOG_H
#define MS_LOG_H

/**
 * @file
 * @defgroup log Logging
 * @{
 * Logging functions and helper macros for SBX for C
 * The macro forms capture the current value of the __FILE__ and __LINE__ macros
 * and call out to the base logging function.
 */

#include <stdio.h>
#include "msys/ms_config.h"

/**
 * Enumeration for basic log levels.
 */
typedef enum ms_log_level ms_log_level_t;
enum ms_log_level {
    ms_debug,   /**< Debugging information. */
    ms_warn,    /**< An unusual/noteworthy condition but the program may continue. */
    ms_fatal    /**< The executing function has put the program into an unsafe state. */
};

void ms_log_setfile(FILE* newfile);
void ms_log_setlevel(ms_log_level_t level);
ms_log_level_t ms_log_getlevel();

void ms_log(ms_log_level_t level, const char* fname, unsigned int line, const char* message, ...);
void ms_log_bytes(ms_log_level_t level, uint32_t count, uint8_t* bytes);

/**
 * If DEBUG is set enable debug logging.
 */
#ifdef DEBUG

/** Log a debug message. */
#define ms_log_debug(message,...) ms_log(ms_debug, __FILE__, __LINE__, message, ##__VA_ARGS__)

#else
/** Noop version of debug logging macro. */
#define ms_log_debug(message,...) 
#endif

/** Log a warning message. */
#define ms_log_warn(message,...) ms_log(ms_warn, __FILE__, __LINE__, message, ##__VA_ARGS__)

/** Log a fatal message. */
#define ms_log_fatal(message,...) ms_log(ms_fatal, __FILE__, __LINE__, message, ##__VA_ARGS__)

/** @} */

#endif
