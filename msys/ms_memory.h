#ifndef MS_MEMORY_H
#define MS_MEMORY_H

/**
 * @file
 * @defgroup mem Memory
 * @{
 * Wrappers for standand C memory management functions:  free, malloc, calloc, realloc.
 * This allows custom allocators to be use in the library.
 *
 * Caution! Using a custom allocation macro must allways be mached by the free macro.
 * Mixing custom and standard allocators is not allowed.
 */

#include <stdlib.h>

/** Free memory. */
#define ms_free(ptr) free(ptr)

/** Alloc fixed size block. */
#define ms_malloc(size) malloc(size)

/** Alloc and clear a fixed size block. */
#define ms_calloc(count, size) calloc(count, size)

/** Reallocate and existing block to a new size. */
#define ms_realloc(ptr, size) realloc(ptr, size)

/** @} */

#endif
