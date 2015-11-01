#ifndef MC_OPTION_H
#define MC_OPTION_H

/**
 * @file
 * @defgroup option CoAP Option
 * @{
 */

#include "mcoap/mc_buffer.h"

#define OPTION_IF_MATCH			1		// opaque 0-8
#define OPTION_URI_HOST			3		// string 1-255
#define OPTION_ETAG				4		// opaque 1-8
#define OPTION_IF_NONE_MATCH	5		// empty  0
#define OPTION_URI_PORT			7		// uint   0-2
#define OPTION_LOCATION_PATH	8       // string 0-255
#define OPTION_URI_PATH			11		// string 0-255
#define OPTION_CONTENT_FORMAT	12		// uint   0-2
#define OPTION_MAX_AGE			14		// uint   0-4
#define OPTION_URI_QUERY		15      // string 0-255
#define OPTION_ACCEPT			17		// uint   0-2
#define OPTION_LOCATION_QUERY	20		// string 0-255
#define OPTION_PROXY_URI		35		// string 1-1034
#define OPTION_PROXY_SCHEME		39		// string 1-255
#define OPTION_SIZE_1			60		// uint   0-4

#define CONTENT_TEXT_PLAIN		0
#define CONTENT_APP_LINK		40
#define CONTENT_APP_XML			41
#define CONTENT_OCTET			42
#define CONTENT_EXI				47
#define CONTENT_JSON			50

/** In memory (vs on-the-wire) option value. */
typedef struct mc_option mc_option_t;
struct mc_option {
    uint16_t option_num;
    mc_buffer_t value;
};

mc_option_t* mc_option_alloc();
mc_option_t* mc_option_nalloc(uint32_t count);
mc_option_t* mc_option_deinit(mc_option_t* option);
mc_option_t* mc_option_ndeinit(mc_option_t* option, uint32_t count);
mc_option_t* mc_option_init(mc_option_t* option, uint16_t option_num, uint32_t nbytes, uint8_t* bytes);
mc_option_t* mc_option_init_uint32(mc_option_t* option, uint16_t option_num, uint32_t value);
mc_option_t* mc_option_init_str(mc_option_t* option, uint16_t option_num, char* value);
mc_option_t* mc_option_copy_to(mc_option_t* to, mc_option_t* from);
uint32_t mc_option_as_uint32(const mc_option_t* option);
uint32_t mc_option_buffer_size(const mc_option_t* option, uint32_t prev_option_num);

/** @} */
#endif
