cmake_minimum_required(VERSION 3.3)
project(testmc)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

set(SOURCE_FILES
    mc_code_test.c
    mc_code_test.h
    mc_endpt_udp_test.c
    mc_endpt_udp_test.h
    mc_header_test.c
    mc_header_test.h
    mc_message_test.c
    mc_message_test.h
    mc_options_list_test.c
    mc_options_list_test.h
    mc_test_main.c
    mc_uri_test.c
    mc_uri_test.h)

add_executable(testmc ${SOURCE_FILES})
add_dependencies(testmc cutest msys mnet mcoap)
target_link_libraries(testmc mcoap mnet msys cutest pthread m)