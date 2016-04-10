cmake_minimum_required(VERSION 3.2)
project(mcoap)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

set(SOURCE_FILES
    mc_buffer.c
    mc_buffer.h
    mc_buffer_queue.c
    mc_buffer_queue.h
    mc_code.c
    mc_code.h
    mc_endpt_udp.c
    mc_endpt_udp.h
    mc_header.c
    mc_header.h
    mc_message.c
    mc_message.h
    mc_option.c
    mc_option.h
    mc_options_list.c
    mc_options_list.h
    mc_token.c
    mc_token.h
    mc_uri.c
    mc_uri.h)

add_library(mcoap ${SOURCE_FILES})
