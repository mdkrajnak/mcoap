#ifndef MCOAP_ACCESSOR_H
#define MCOAP_ACCESSOR_H

#include "mcoap/mc_endpt_udp.h"

mc_buffer_t* mc_endpt_udp_copy_queued_token(mc_endpt_udp_t* endpt, uint16_t msgid);

#endif
