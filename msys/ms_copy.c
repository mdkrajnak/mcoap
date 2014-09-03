#include <stdlib.h>
#include <string.h>

#include "msys/ms_copy.h"

/**
 * @file
 * @ingroup copy_util 
 * @{
 * This file contains the implementations for the functions defined in us_copy_util.h.
 */

/** Allocate and copy a null terminated string. */
char* us_copy_str(const char* str) {
    char* result = 0;
    size_t len;
 
    if (str == 0) return result;
    len = strlen(str) + 1;
    
    result = (char*)malloc(len);
    return strncpy(result, str, len);
}

/** Allocate and copy an array of chars given a count of the data with null termination. */
char* us_copy_cstr(uint32_t count, const char* data) {
    char* result = (char*)calloc(count + 1, sizeof(char));
    memcpy(result, data, count);
    return result;
}

/** Allocate and copy an array of chars given a count of the data (not null terminated). */
char* us_copy_char(uint32_t count, const char* data) {
    size_t len = count * sizeof(char);
    char* result = (char*)malloc(len);
    memcpy(result, data, len);
    return result;
}

/** 
 * Allocate and copy an array of chars assuming utf-8 encoding.
 * Implementation-wise this can be identical to copy for chars.
 */
char* us_copy_utf8(uint32_t count, const char* data)  {
    size_t len = sizeof(char)*count;
    char* result = (char*)malloc(len);
    memcpy(result, data, len);
    return result;
}

/** Allocate and copy an array of int8's. */
int8_t* us_copy_int8(uint32_t count, const int8_t* data) {
    size_t len = count * sizeof(int8_t);
    int8_t* result = (int8_t*)malloc(len);
    memcpy(result, data, len);
    return result;
}

/** Allocate and copy an array of int16's. */
int16_t* us_copy_int16(uint32_t count, const int16_t* data) {
    size_t len = count * sizeof(int16_t);
    int16_t* result = (int16_t*)malloc(len);
    memcpy(result, data, len);
    return result;
}

/** Allocate and copy an array of int32's. */
int32_t* us_copy_int32(uint32_t count, const int32_t* data) {
    size_t len = count * sizeof(int32_t);
    int32_t* result = (int32_t*)malloc(len);
    memcpy(result, data, len);
    return result;
}

/** Allocate and copy an array of int64's. */
int64_t* us_copy_int64(uint32_t count, const int64_t* data) {
    size_t len = count * sizeof(int64_t);
    int64_t* result = (int64_t*)malloc(len);
    memcpy(result, data, len);
    return result;
}

/** Allocate and copy an array of uint8's. */
uint8_t* us_copy_uint8(uint32_t count, const uint8_t* data) {
    size_t len = count * sizeof(uint8_t);
    uint8_t* result = (uint8_t*)malloc(len);
    memcpy(result, data, len);
    return result;
}

/** Allocate and copy an array of uint16's. */
uint16_t* us_copy_uint16(uint32_t count, const uint16_t* data) {
    size_t len = count * sizeof(uint16_t);
    uint16_t* result = (uint16_t*)malloc(len);
    memcpy(result, data, len);
    return result;
}

/** Allocate and copy an array of uint32's. */
uint32_t* us_copy_uint32(uint32_t count, const uint32_t* data) {
    size_t len = count * sizeof(uint32_t);
    uint32_t* result = (uint32_t*)malloc(len);
    memcpy(result, data, len);
    return result;
}

/** Allocate and copy an array of uint64's. */
uint64_t* us_copy_uint64(uint32_t count, const uint64_t* data) {
    size_t len = count * sizeof(uint64_t);
    uint64_t* result = (uint64_t*)malloc(len);
    memcpy(result, data, len);
    return result;
}

/** 
 * Allocate and copy an array of uint32 assuming ntp32 encoding.
 * Implementation wise this is identical to copying uint32's.
 */
uint32_t* us_copy_ntp32(uint32_t count, const uint32_t* data) {
    size_t len = count * sizeof(uint32_t);
    uint32_t* result = (uint32_t*)malloc(len);
    memcpy(result, data, len);
    return result;
}

/** 
 * Allocate and copy an array of uint64 assuming ntp64 encoding.
 * Implementation wise this is identical to copying uint32's.
 */
uint64_t* us_copy_ntp64(uint32_t count, const uint64_t* data) {
    size_t len = count * sizeof(uint64_t);
    uint64_t* result = (uint64_t*)malloc(len);
    memcpy(result, data, len);
    return result;
}

#ifdef US_HAS_SINGLE_FLOAT
/** Allocate and copy an array of float32's. */
float* us_copy_float32(uint32_t count, const float* data) {
    size_t len = count * sizeof(float);
    float* result = (float*)malloc(len);
    memcpy(result, data, len);
    return result;
}
#endif

#ifdef US_HAS_DOUBLE_FLOAT
/** Allocate and copy an array of float64's. */
double* us_copy_float64(uint32_t count, const double* data) {
    size_t len = count * sizeof(double);
    double* result = (double*)malloc(len);
    memcpy(result, data, len);
    return result;
}
#endif

/** @} */

