cmake_minimum_required(VERSION 3.3)
project(testms)

set(SOURCE_FILES
    ms_endian_test.c
    ms_endian_test.h
    ms_test_main.c)

add_executable(testms ${SOURCE_FILES})
add_dependencies(testms cutest msys)
target_link_libraries(testms cutest msys)