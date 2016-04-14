cmake_minimum_required(VERSION 3.2)
project(tmserver)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

set(SOURCE_FILES
    tmserver.c)

add_executable(tmserver ${SOURCE_FILES})
add_dependencies(tmserver msys mnet mcoap)
target_link_libraries(tmserver mcoap mnet msys pthread m)
