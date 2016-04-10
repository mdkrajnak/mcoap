cmake_minimum_required(VERSION 3.2)
project(msys)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

set(SOURCE_FILES
    ms_config.h
    ms_copy.c
    ms_copy.h
    ms_endian.c
    ms_endian.h
    ms_log.c
    ms_log.h
    ms_memory.h
    ms_mutex.c
    ms_mutex.h
    ms_thread.c
    ms_thread.h)

add_library(msys ${SOURCE_FILES})
