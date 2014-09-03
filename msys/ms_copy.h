#ifndef MS_COPY_H
#define MS_COPY_H

/**
 * @file
 * @defgroup copy_util Copy Utils
 * @{
 * This unit contains utilities that takes pointers to fundamental types and
 * arrays of fundamental types, allocates copies, and returns pointers to them.
 *
 * The primary purpose is to provide simple wrappers for the arguments to 
 * initialization functions.  Initializer functions in general when given a 
 * pointer store the pointer and not a copy of the data.  If the argument
 * is wrapped in a copy function then the initializer stores a copy.
 */

#include "msys/ms_config.h"

char* ms_copy_str(const char* str);

char* ms_copy_char(uint32_t count, const char* data);
char* ms_copy_cstr(uint32_t count, const char* data);
char* ms_copy_utf8(uint32_t count, const char* data);

int8_t* ms_copy_int8(uint32_t count, const int8_t* data);
int16_t* ms_copy_int16(uint32_t count, const int16_t* data);
int32_t* ms_copy_int32(uint32_t count, const int32_t* data);
int64_t* ms_copy_int64(uint32_t count, const int64_t* data);

uint8_t* ms_copy_uint8(uint32_t count, const uint8_t* data);
uint16_t* ms_copy_uint16(uint32_t count, const uint16_t* data);
uint32_t* ms_copy_uint32(uint32_t count, const uint32_t* data);
uint64_t* ms_copy_uint64(uint32_t count, const uint64_t* data);

/* 
 * ms_config.h defines ms_HAS_SINGLE_FLOAT and ms_HAS_DOUBLE_FLOAT depending
 * on the platform/compiler.
 */
#ifdef MS_HAS_SINGLE_FLOAT
float* ms_copy_float32(uint32_t count, const float* data);
#endif

#ifdef MS_HAS_DOUBLE_FLOAT
double* ms_copy_float64(uint32_t count, const double* data);
#endif

/** @} */

#endif
