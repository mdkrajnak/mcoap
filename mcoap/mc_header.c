/**
 * @file
 * @ingroup header
 * @{
 */
 
#include "mcoap/mc_header.h"

#define VERSION_MASK 0xB0000000
#define MTYPE_MASK   0x30000000
#define TKLEN_MASK   0x0F000000
#define CODE_MASK    0x00FF0000
#define MID_MASK     0x0000FFFF

#define VERSION_OFFSET 30
#define MTYPE_OFFSET   28
#define TKLEN_OFFSET   24
#define CODE_OFFSET    16

/**
 * Given the header components format the header.
 * Note that the arguments are not checked to see if they are valid.
 */
uint32_t mc_header_create(uint8_t version, uint8_t message_type, uint32_t token_len, uint8_t code, uint16_t message_id) {
    uint32_t header = (VERSION_MASK & (version << VERSION_OFFSET));
    header = header | (MTYPE_MASK & (message_type << MTYPE_OFFSET));
    header = header | (TKLEN_MASK & (token_len << TKLEN_OFFSET));
    header = header | (CODE_MASK & (code << CODE_OFFSET));
    header = header | (MID_MASK & message_id);
	return header;
}

/**
 * Given a header get the version.
 */
uint8_t mc_header_get_version(uint32_t header) {
	return (VERSION_MASK & header) >> VERSION_OFFSET;
}

/**
 * Given a header get the version.
 */
uint8_t mc_header_get_message_type(uint32_t header) {
	return (MTYPE_MASK & header) >> MTYPE_OFFSET;
}

/**
 * Given a header get the version.
 */
uint8_t mc_header_get_token_length(uint32_t header) {
	return (TKLEN_MASK & header) >> TKLEN_OFFSET;
}

/**
 * Given a header get the version.
 */
uint8_t mc_header_get_code(uint32_t header) {
	return (CODE_MASK & header) >> CODE_OFFSET;
}

/**
 * Given a header get the version.
 */
uint16_t mc_header_get_message_id(uint32_t header) {
	return MID_MASK & header;
}

/** @} */
