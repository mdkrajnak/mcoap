cmake_minimum_required(VERSION 3.3)
project(testbase)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

set(SOURCE_FILES
    testbase.c
    core01.h
    core01.c)

add_executable(testbase ${SOURCE_FILES})
add_dependencies(testbase cutest msys mnet mcoap)
target_link_libraries(testbase mcoap mnet msys cutest pthread m)