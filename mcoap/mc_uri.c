/**
 * @file
 * @ingroup uri
 * @{
 */
#include "msys/ms_memory.h"
#include "msys/ms_copy.h"
#include "mnet/mn_socket.h"
#include "mcoap/mc_uri.h"
#include <string.h>


/** Extract the scheme from the URI. */
static char* getscheme(char* const uri) {
	size_t schemelen;
	char* colon;

	colon = strstr(uri, ":");
	if (!colon) return 0;

	schemelen = colon - uri;
	return ms_copy_cstr(schemelen, uri);
}

/** Valid if it is "coap" or "coaps". */
static int validscheme(char* const scheme) {
	if (scheme == 0) return 0;
	if (strcmp(scheme, "coap") == 0) return 1;
	if (strcmp(scheme, "coaps") == 0) return 1;
	return 0;
}

/**
 * Return a pointer into str offset by prefix.
 * prefix *must* match the contents of str character-by-character,
 * otherwise its an error.
 * @return pointer in str advanced by length of prefix or 0 on error.
 */
static char* advanceby(char* const str, char* const prefix) {
	char* spos = str;
	char* ppos = prefix;

	if (!str || !prefix) return 0;

	while (*ppos) {
		if (*spos == 0) return 0;
		if (*spos != *ppos) return 0;
		spos++;
		ppos++;
	}
	return spos;
}

/**
 * Get an IP v6 host by extracting anything between []'s.
 */
static char* getip6host(char* const uri) {
	char* current;
	size_t nchar;

	if (!uri) return 0;

	current = advanceby(uri, "[");
	nchar = strcspn(current, "]");

	if (nchar == 0) return 0;
	if (current[nchar] != ']') return 0;

	return ms_copy_cstr(nchar, current);
}

/**
 * Get any characters from start of URI to next : or /.
 * Won't work for IP v6, treat it as a special case.
 */
static char* gethost(char* const uri) {
	size_t nchar;

	if (!uri) return 0;

	nchar = strcspn(uri, ":/\0");
	return ms_copy_cstr(nchar, uri);
}

/** Get host address from URI, IP v6 is a special case. */
static char* getaddress(char* const uri) {
	char* address;

	if (*uri == '[') {
		address = getip6host(uri);
	}
	else {
		address = gethost(uri);
	}
	return address;
}

/**
 * Get port from URI.
 * Note: If port is invalid with respect to atoi conversions this
 * function still returns the default port.
 * @return port or 0 if none.
 */
static uint32_t getport(char* const uri) {
	size_t nchar;
	char* current;
	char* portstr;
	uint32_t port;

	if (!uri) return 0;
	if (*uri != ':') return MC_DEFAULT_PORT;

	current = advanceby(uri, ":");
	nchar = strcspn(current, "/\0");
	portstr = ms_copy_cstr(nchar, current);
	port = atoi(portstr);
	if (!port) port = MC_DEFAULT_PORT;

	ms_free(portstr);
	return port;
}

/**
 * Advance to next character in chars, if *not* found advances
 * to end-of-string.
 * @return pointer to first found string character or end of string.
 */
static char* advanceto(char* const str, char* const chars) {
	size_t nbytes = strcspn(str, chars);
	if (nbytes == 0) nbytes = strlen(str);
	return str + nbytes;
}

/**
 * Estimate the number of path and query options needed by counting /, ?, and & characters.
 * @return the number of estimated options.
 */
static uint32_t estimatenopts(char* const str) {
	uint32_t nopts = 0;
	char* pos = (char*)str;

	while (*pos) {
		if ((*pos == '/') || (*pos == '?') || (*pos == '&'))
			nopts++;
		pos++;
	}
	return nopts;
}

/**
 * Get next path componenent.
 * Must begin with / and goes up until next /, ? or end of line.
 * @return path or 0 if no more.
 */
static char* getpath(char* const uri) {
	size_t nchar;
	char* current;

	if (!uri) return 0;
	if (*uri != '/') return 0;

	current = advanceby(uri, "/");
	nchar = strcspn(current, "/?");
	return ms_copy_cstr(nchar, current);
}

/**
 * Get next query component.
 * Must begin with ? or & and goes up until next & or end of line.
 * @return path or 0 if no more.
 */
static char* getquery(char* const uri) {
	size_t nchar;
	char* current = (char*)uri;

	if (!current) return 0;
	if ((*current != '?') && (*current != '&')) return 0;

	current++;
	nchar = strcspn(current, "&");
	return ms_copy_cstr(nchar, current);
}



static int equaladdr(sockaddr_t* const left, sockaddr_t* const right) {
	int result;

	/* If one address is null and the other is not return false. */
	if (left && !right) return 0;
	if (!left && right) return 0;

	/* if both are null return true. */
	if (!left && !right) return 1;
	if (left->sa_family != right->sa_family) return 0;
	if (left->sa_family == AF_INET) {
		result = (0 == memcmp(left, right, sizeof(struct sockaddr_in)));
	}
	else {
		result = (0 == memcmp(left, right, sizeof(struct sockaddr_in6)));
	}

	return result;
}

/**
 * Parse host, port, path, and query components from uri.
 * If destination host and/or port are in the URI they are elided
 * from the options list.
 *
 * @todo Does not reject URI's with fragments (RFC7252 6.4 Step 4).
 * @todo Does perform percent-decoding.
 *
 * Note: Host is elided if it matches desthost, RFC7252 says this only
 * happens if desthost is IP v6 or v4 literal. We rely on the caller
 * to only pass in the desthost string if it is an IP literal.
 *
 * @return the corresponding options list or 0 on error.
 */
mc_options_list_t* mc_uri_to_options(mc_options_list_t* const list, sockaddr_t* const dest, char* const uri) {
	mc_option_t* options;
	sockaddr_t uriaddr;
	char* host;
	char* path;
	char* query;
	uint32_t port;
	uint32_t nopts;
	uint32_t iopt;

	char* current = (char*)uri;
	char* scheme = getscheme(current);
	if (!validscheme(scheme)) {
		if (scheme) ms_free(scheme);
		return 0;
	}
	ms_free(scheme);

	current = advanceto(current, ":");
	current = advanceby(current, "://");
	if (!current) {
		return 0;
	}

	host = getaddress(current);
	if (!host) {
		return 0;
	}

	if (strlen(host) == 0) {
		ms_free(host);
		return 0;
	}

	/* Check and compensate for IP v6 []'s if needed. */
	if (current && (*current == '[')) current += 2;

	current += strlen(host);
	port = getport(current);
	current = advanceto(current, "/?");

	/* Estimate the number of options needed based on remaining path, */
	/* and if the destination matches the URI, then allocate the option array. */
	/* Note the RFC only omits the host if it is an IP literal. */
	nopts = estimatenopts(current);
	mn_sockaddr_inet_init(&uriaddr, host, port);
	if (!equaladdr(&uriaddr, dest)) {
		nopts += 2;
	}
	options = ms_calloc(nopts, mc_option_t);

	/* Note does not properly account for IP literal requirement in section 6.4 step 5. */
	iopt = 0;
	if (!equaladdr(&uriaddr, dest)) {
		mc_option_init_str(&options[iopt], OPTION_URI_HOST, host);
		iopt++;

		mc_option_init_uint32(&options[iopt], OPTION_URI_PORT, port);
		iopt++;
	}

	path = getpath(current);
	while (path) {
		mc_option_init_str(&options[iopt], OPTION_URI_PATH, ms_copy_str(path));
		iopt++;
		current = advanceto(++current, "/?");
		ms_free(path);
		path = getpath(current);
	}

	query = getquery(current);
	while (query) {
		mc_option_init_str(&options[iopt], OPTION_URI_QUERY, ms_copy_str(query));
		iopt++;
		current = advanceto(++current, "&");
		ms_free(query);
		query= getquery(current);
	}

	return mc_options_list_init(mc_options_list_alloc(), iopt, options);
}

/**
 * Initialize a socket address from a coap URI.
 * @return a pointer to the address or 0 on failure.
 */
sockaddr_t* mc_uri_to_address(sockaddr_t* const addr, char* const uri) {
	sockaddr_t* result;
	char* host;
	uint32_t port;

	/* Skip the scheme. */
	char* current = advanceto(uri, ":");
	current = advanceby(current, "://");
	if (!current) {
		return 0;
	}

	host = getaddress(current);
	if (!host) {
		return 0;
	}

	if (strlen(host) == 0) {
		ms_free(host);
		return 0;
	}

	/* Check and compensate for IP v6 []'s if needed. */
	if (current && (*current == '[')) current += 2;

	current += strlen(host);
	port = getport(current);
	if (port == 0) {
		ms_free(host);
		return 0;
	}

	result = mn_sockaddr_inet_init(addr, host, port);
	ms_free(host);

	return result;
}


/** @} */
