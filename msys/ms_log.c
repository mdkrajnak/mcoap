#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>

#include "msys/ms_log.h"

/**
 * @file
 * @ingroup log
 * @{
 * This file implements the logging functions defined in ms_log.h.
 */

/** 
 * Global file pointer to the log file. 
 * Must be initialized before any logging calls.
 */
static FILE* logfile = 0;

/** 
 * Global logging level.
 * Messages less then log level in priority are not logged.
 */
static ms_log_level_t loglevel = ms_fatal;

/**
 * Set the global logfile pointer.
 */
void ms_log_setfile(FILE* newfile) {
    logfile = newfile;
}

/**
 * Set the global log level.
 */
void ms_log_setlevel(ms_log_level_t level) {
    loglevel = level;
}

/**
 * Get the global log level.
 */
ms_log_level_t ms_log_getlevel() {
    return loglevel;
}

/*
 * Get the current log level as a string.
 */
static char* getlevel(ms_log_level_t level) {
    char* result = "DEFAULT";
    switch (level) {
        case ms_debug:
            result = "DEBUG";
            break;
         case ms_warn:
            result = "WARN";
            break;
        case ms_fatal:
            result = "FATAL";
            break;
    }
    return result;
}


/*
 * Clean up the file name for readability.
 * Look for common path seperators, "\" and "/" and strip off proceeding path for clarity.
 */
static const char* getname(const char* fname) {
    char* result;

    result = strrchr(fname, '/');
    if (result) return ++result;

    result = strrchr(fname, '\\');
    if (result) return ++result;

    return fname;
}

/* Define a slightly different form under WinCE because it does not support posix time calls. */
#ifdef WINCE
/**
 * General purpose logging function. "message" must be a printf-style formatting string.
 */
void ms_log(ms_log_level_t level, const char* fname, unsigned int line, const char* message, ...) {
    va_list args;

    if (!logfile) return;
    if (level < loglevel) return;

    /* Don't know how to get formatted time yet on WinCE. */
    fprintf(logfile, "%s@%s:%d ", getlevel(level), getname(fname), line);

    va_start(args, message);
    vfprintf(logfile, message, args);
    va_end(args);

    fprintf(logfile, "\n");
    fflush(logfile);
}

#else
/**
 * General purpose logging function. "message" must be a printf-style formatting string.
 */

void ms_log(ms_log_level_t level, const char* fname, unsigned int line, const char* message, ...) {
    time_t seconds;
    char datebuf[16];
    va_list args;

    if (!logfile) return;
    if (level < loglevel) return;

    seconds = time(0);
    strftime(datebuf, (sizeof datebuf), "%X", localtime(&seconds));
    fprintf(logfile, "%s@%s:%d (%s) ", getlevel(level), getname(fname), line, datebuf);

    va_start(args, message);
    vfprintf(logfile, message, args);
    va_end(args);

    fprintf(logfile, "\n");
    fflush(logfile);
}

#endif

/** @} */
