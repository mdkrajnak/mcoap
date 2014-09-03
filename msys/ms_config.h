#ifndef MS_CONFIG_H
#define MS_CONFIG_H

/**
 * @file 
 * @defgroup config Configuration
 * @{
 * This file defines configuration specific macros, especially with 
 * respect to the different platforms/compilers the libraries may be built on.
 */

#include <stdlib.h>

#ifdef _MSC_VER 
    #if _MSC_VER < 1600
    #include "msys/msstdint.h"
    #else
    #include <stdint.h>
    #endif
#else
#include <stdint.h>
#endif

#if defined(WIN32) && defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC 1
#endif

/** Test to see if this specific compiler supports 4 byte floats. */
#define MS_HAS_SINGLE_FLOAT sizeof(float) == 4

/** Test to see if this specific compiler supports 8 byte doubles. */
#define MS_HAS_DOUBLE_FLOAT sizeof(double) == 8

/**
 * If we do NOT have 4 byte floats we substitute 4 byte unsigned ints for floats.
 * This is necessary to be able to parse over such types in data buffers.
 * However we try to discourage use on machines without such support by wrapping
 * the relevant convience methods in the appropriate MS_HAS_XXX_FLOAT macros.
 */
#ifdef MS_HAS_SINGLE_FLOAT
typedef float float32_t;
#else
typedef uint32_t float32_t;
#endif

/** Do the same for double precision. */
#ifdef MS_HAS_DOUBLE_FLOAT
typedef double float64_t;
#else
typedef uint64_t float64_t;
#endif

/* VC++ defines _DEBUG if /MTd or /Md is set, take advantage of this to simplify vcproj files. */
#ifdef _DEBUG
#ifndef DEBUG
#define DEBUG 1
#endif
#endif

/** @} */
#endif 
