cmake_minimum_required(VERSION 3.3)
project(testbase)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

set(SOURCE_FILES
    accessor.h
    accessor.c
    compare.h
    compare.c
    print.h
    print.c
    core01_get_con.h
    core01_get_con.c
    core02_delete_con.h
    core02_delete_con.c
    testbase.c)

add_executable(testbase ${SOURCE_FILES})
add_dependencies(testbase cutest msys mnet mcoap)
target_link_libraries(testbase mcoap mnet msys cutest pthread m)