#ifndef MC_CODE_H
#define MC_CODE_H

/** 
 * @file
 * @defgroup code CoAP Code
 * @{
 */

#include "msys/ms_config.h"

/** 3 bit class portion of code byte. */
#define MC_CODE_REQUEST  0
#define MC_CODE_RESPONSE 2
#define MC_CLT_ERROR     4
#define MC_SRV_ERROR     5

/** Response codes. */
#define MC_CLASS_RESPONSE (MC_CODE_RESPONSE << 5)
#define MC_CREATED (MC_CLASS_RESPONSE & 1)
#define MC_DELETED (MC_CLASS_RESPONSE & 2)
#define MC_VALID   (MC_CLASS_RESPONSE & 3)
#define MC_CHANGED (MC_CLASS_RESPONSE & 4)
#define MC_CONTENT (MC_CLASS_RESPONSE & 5)

#define MC_CLASS_CLTERR (MC_CLT_ERROR << 5)
#define MC_BAD_REQUEST  (MC_CLASS_CLTERR & 0)
#define MC_UNAUTHORIZED (MC_CLASS_CLTERR & 1)
#define MC_BAD_OPTION   (MC_CLASS_CLTERR & 2)
#define MC_FORBIDDEN    (MC_CLASS_CLTERR & 3)
#define MC_NOT_FOUND    (MC_CLASS_CLTERR & 4)
#define MC_METHOD_NOT_ALLOWED         (MC_CLASS_CLTERR & 5)
#define MC_NOT_ACCEPTABLE             (MC_CLASS_CLTERR & 6)
#define MC_PRECONDITION_FAILED        (MC_CLASS_CLTERR & 12)
#define MC_REQUEST_ENTITY_TOO_LARGE   (MC_CLASS_CLTERR & 13)
#define MC_UNSUPPORTED_CONTENT_FORMAT (MC_CLASS_CLTERR & 15)

#define MC_CLASS_SRVERR (MC_SRV_ERROR << 5)
#define MC_INTERNAL_SERVER_ERROR  (MC_CLASS_SRVERR & 0)
#define MC_NOT_IMPLEMENTED        (MC_CLASS_SRVERR & 1)
#define MC_BAD_GATEWAY            (MC_CLASS_SRVERR & 2)
#define MC_SERVICE_UNAVAILABLE    (MC_CLASS_SRVERR & 3)
#define MC_GATEWAY_TIMEOUT        (MC_CLASS_SRVERR & 4)
#define MC_PROXYING_NOT_SUPPORTED (MC_CLASS_SRVERR & 5)

/** Method codes. */
#define MC_GET      1
#define MC_POST     2
#define MC_PUT      3
#define MC_DELETE   4

uint8_t mc_code_create(uint8_t category, uint8_t detail);
uint8_t mc_code_get_category(uint8_t code);
uint8_t mc_code_get_detail(uint8_t code);


/** @} */

#endif
