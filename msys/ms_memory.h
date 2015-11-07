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

/**
 * Free memory.
 * Would like to add a ptr = 0 to this macro to avoid freeing const ptr's but
 * that conflicts with some idioms used the library e.g. ms_free(some_type_deinit(ptr));
 * Where some_type_deinit() returns ptr to be freed.
 */
#define ms_free(ptr) \
    do { \
        free(ptr); \
    } while (0)

/** Alloc fixed size block. */
#define ms_malloc(count, decl) (decl*)malloc(count * sizeof(decl))

/** Alloc and clear a fixed size block. */
#define ms_calloc(count, decl) (decl*)calloc(count, sizeof(decl))

/** Reallocate and existing block to a new size. */
#define ms_realloc(ptr, count, decl) (decl*)realloc(ptr, count*sizeof(decl))

/** @} */

#endif
