#include <string.h>
#include "msys/ms_memory.h"
#include "mnet/mn_sockaddr.h"

/**
 * @file
 * @ingroup socket
 * @{
 * Socket address utilities.
 */

/**
 * Convert a host name to an in_addr structure.
 * First assume its an IPv4 decimal dotted address, if not
 * try looking it up by name.
 * @return MN_DONE on success.
 */
static int host2addr(struct in_addr* addr, const char* hostname) {
    int err;
    int success;

    /* Try to convert from decimal dotted form to address. */
    success = inet_pton(AF_INET, hostname, addr);

    /* If conversion failed assume it is a hostname and look it up. */
    if (!success) {
        hostent_t* hostent = NULL;
        in_addr_t** inaddr;

        err = mn_gethostbyname(hostname, &hostent);
        if (err != MN_DONE) return err;

        inaddr = (in_addr_t**)hostent->h_addr_list;
        memcpy(addr, *inaddr, sizeof(in_addr_t));
    }
    return MN_DONE;
}

/**
 * Create a copy of src.
 * @return the copy.
 */
sockaddr_t* mn_sockaddr_copy(sockaddr_t* src) {
    sockaddr_t* result = ms_calloc(1, sockaddr_t);
    return (sockaddr_t*)memcpy(result, src, sizeof(sockaddr_t));
}

/**
 * Initialize an internet address structure.
 * If the hostname is the * wildcard we use INADDR_ANY for the address.
 * @return pointer to the initialized address or 0.
 */
sockaddr_t* mn_sockaddr_inet_init(sockaddr_t* addr, const char *hostname, unsigned short port) {
    int err;
    struct sockaddr_in* inaddr;

    if (!addr) return 0;
    inaddr = (struct sockaddr_in*)addr;

    inaddr->sin_port = htons(port);

    if (strcmp(hostname, "*")) {
        inaddr->sin_family = AF_INET;
        err = host2addr(&inaddr->sin_addr, hostname);
        if (err != MN_DONE) return 0;
    }
    else {
        inaddr->sin_addr.s_addr = htonl(INADDR_ANY);
    }
    return addr;
}

/** @} */
