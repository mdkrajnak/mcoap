#include "cutest/CuTest.h"
#include "mcoap/mc_option.h"
#include "mcoap/mc_uri.h"
#include "msys/ms_memory.h"
#include <string.h>

#include "testmc/mc_uri_test.h"

/**
 *  Given a URI with just host:port
 *  when we create an option list
 *  then it has that host and port.
 */
static void test_address_with_only_host(CuTest* tc) {

	mc_options_list_t* options = mc_uri_to_options(mc_options_list_alloc(), 0, "coap://127.0.0.1");

	int hindex = mc_options_list_get_index(options, 0, OPTION_URI_HOST);
	int pindex = mc_options_list_get_index(options, 0, OPTION_URI_PORT);
	mc_option_t* hopt = mc_options_list_at(options, hindex);
	mc_option_t* popt = mc_options_list_at(options, pindex);


	CuAssert(tc, "is parsable", options != 0);
	CuAssert(tc, "has host", hindex > -1);
	CuAssert(tc, "has port", pindex > -1);
	CuAssert(tc, "host is 127.0.0.1", strncmp("127.0.0.1", (char*)hopt->value.bytes, hopt->value.nbytes) == 0);
	CuAssert(tc, "port is 5683", mc_option_as_uint32(popt) == MC_DEFAULT_PORT);

	ms_free(mc_options_list_deinit(options));
}

/**
 *  Given a URI with just host:port
 *  when we create an option list
 *  then it has that host and port.
 */
static void test_address_with_host_and_port(CuTest* tc) {

	mc_options_list_t* options = mc_uri_to_options(mc_options_list_alloc(), 0, "coap://localhost:1000");

	int hindex = mc_options_list_get_index(options, 0, OPTION_URI_HOST);
	int pindex = mc_options_list_get_index(options, 0, OPTION_URI_PORT);
	mc_option_t* hopt = mc_options_list_at(options, hindex);
	mc_option_t* popt = mc_options_list_at(options, pindex);


	CuAssert(tc, "is parsable", options != 0);
	CuAssert(tc, "has host", hindex > -1);
	CuAssert(tc, "has port", pindex > -1);
	CuAssert(tc, "host is localhost", strncmp("localhost", (char*)hopt->value.bytes, hopt->value.nbytes) == 0);
	CuAssert(tc, "port is 1000", mc_option_as_uint32(popt) == 1000);

	ms_free(mc_options_list_deinit(options));
}

/**
 *  Given a URI with an ip v6 address
 *  when we create an option list
 *  then it has that host and port.
 */
static void test_address_ipv6(CuTest* tc) {

	mc_options_list_t* options = mc_uri_to_options(mc_options_list_alloc(), 0, "coap://[fe80::225:11ff:fe3e:6904]:2000");

	int hindex = mc_options_list_get_index(options, 0, OPTION_URI_HOST);
	int pindex = mc_options_list_get_index(options, 0, OPTION_URI_PORT);
	mc_option_t* hopt = mc_options_list_at(options, hindex);
	mc_option_t* popt = mc_options_list_at(options, pindex);


	CuAssert(tc, "is parsable", options != 0);
	CuAssert(tc, "has host", hindex > -1);
	CuAssert(tc, "has port", pindex > -1);
	CuAssert(tc, "host is fe80::225:11ff:fe3e:69041", strncmp("fe80::225:11ff:fe3e:6904", (char*)hopt->value.bytes, hopt->value.nbytes) == 0);
	CuAssert(tc, "port is 2000", mc_option_as_uint32(popt) == 2000);

	ms_free(mc_options_list_deinit(options));
}

/**
 *  Given a URI with an ip v6 address
 *  when we create an option list
 *  then it has that host and port.
 */
static void test_address_invalid_ipv6(CuTest* tc) {

	mc_options_list_t* options = mc_uri_to_options(mc_options_list_alloc(), 0, "coap://[fe80:2000");

	CuAssert(tc, "is not parsable", options == 0);
}

/**
 *  Given a URI without a host,
 *  when we create an option list
 *  then it is not parsed.
 */
static void test_address_with_missing_host(CuTest* tc) {

	mc_options_list_t* options = mc_uri_to_options(mc_options_list_alloc(), 0, "coap://:1000");

	CuAssert(tc, "is not parsable", options == 0);
}

/**
 *  Given a URI with an invalid scheme
 *  when we create an option list
 *  then it is not parsed.
 */
static void test_address_with_invalid_scheme(CuTest* tc) {

	mc_options_list_t* options = mc_uri_to_options(mc_options_list_alloc(), 0, "cop://localhost:1000");

	CuAssert(tc, "is not parsable", options == 0);
}

/**
 *  Given a URI with  host:port/path
 *  when we create an option list
 *  then it has that host, port, path, and query
 */
static void test_address_with_path(CuTest* tc) {

	mc_options_list_t* options = mc_uri_to_options(mc_options_list_alloc(), 0, "coap://127.0.0.1:1000/path/to/resource");

	int hindex = mc_options_list_get_index(options, 0, OPTION_URI_HOST);
	int pindex = mc_options_list_get_index(options, 0, OPTION_URI_PORT);
	int rindex = mc_options_list_get_index(options, 0, OPTION_URI_PATH);
	int qindex = mc_options_list_get_index(options, 0, OPTION_URI_QUERY);
	mc_option_t* hopt = mc_options_list_at(options, hindex);
	mc_option_t* popt = mc_options_list_at(options, pindex);
	mc_option_t* ropt = mc_options_list_at(options, rindex);


	CuAssert(tc, "is parseable", options != 0);
	CuAssert(tc, "host is 127.0.0.1", strncmp("127.0.0.1", (char*)hopt->value.bytes, hopt->value.nbytes) == 0);
	CuAssert(tc, "port is 1000", mc_option_as_uint32(popt) == 1000);

	CuAssert(tc, "path1 is path", strncmp("path", (char*)ropt->value.bytes, ropt->value.nbytes) == 0);
	ropt++;

	CuAssert(tc, "path2 is to", strncmp("to", (char*)ropt->value.bytes, ropt->value.nbytes) == 0);
	ropt++;

	CuAssert(tc, "path3 is resource", strncmp("resource", (const char*)ropt->value.bytes, ropt->value.nbytes) == 0);
	CuAssert(tc, "no query present", qindex == -1);

	ms_free(mc_options_list_deinit(options));
}

/**
 *  Given a URI with  host:port/path?query
 *  when we create an option list
 *  then it has that host, port, path, and query
 */
static void test_address_with_path_and_query(CuTest* tc) {

	mc_options_list_t* options = mc_uri_to_options(mc_options_list_alloc(), 0, "coap://127.0.0.1:1000/path/to/resource?q1&q2");

	int hindex = mc_options_list_get_index(options, 0, OPTION_URI_HOST);
	int pindex = mc_options_list_get_index(options, 0, OPTION_URI_PORT);
	int rindex = mc_options_list_get_index(options, 0, OPTION_URI_PATH);
	int qindex = mc_options_list_get_index(options, 0, OPTION_URI_QUERY);
	mc_option_t* hopt = mc_options_list_at(options, hindex);
	mc_option_t* popt = mc_options_list_at(options, pindex);
	mc_option_t* ropt = mc_options_list_at(options, rindex);
	mc_option_t* qopt = mc_options_list_at(options, qindex);


	CuAssert(tc, "is parseable", options != 0);
	CuAssert(tc, "host is 127.0.0.1", strncmp("127.0.0.1", (char*)hopt->value.bytes, hopt->value.nbytes) == 0);
	CuAssert(tc, "port is 1000", mc_option_as_uint32(popt) == 1000);

	CuAssert(tc, "path1 is path", strncmp("path", (char*)ropt->value.bytes, ropt->value.nbytes) == 0);
	CuAssert(tc, "query1 is q1", strncmp("q1", (char*)qopt->value.bytes, qopt->value.nbytes) == 0);
	ropt++;
	qopt++;

	CuAssert(tc, "path2 is to", strncmp("to", (char*)ropt->value.bytes, ropt->value.nbytes) == 0);
	CuAssert(tc, "query2 is q2", strncmp("q2", (char*)qopt->value.bytes, qopt->value.nbytes) == 0);
	ropt++;

	CuAssert(tc, "path3 is resource", strncmp("resource", (const char*)ropt->value.bytes, ropt->value.nbytes) == 0);

	ms_free(mc_options_list_deinit(options));
}

/* Run all of the tests in this test suite. */
CuSuite* mc_uri_suite() {
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, test_address_with_only_host);
    SUITE_ADD_TEST(suite, test_address_with_host_and_port);
    SUITE_ADD_TEST(suite, test_address_with_missing_host);
    SUITE_ADD_TEST(suite, test_address_ipv6);
    SUITE_ADD_TEST(suite, test_address_invalid_ipv6);
    SUITE_ADD_TEST(suite, test_address_with_invalid_scheme);
    SUITE_ADD_TEST(suite, test_address_with_path);
    SUITE_ADD_TEST(suite, test_address_with_path_and_query);

        
    return suite;
}
