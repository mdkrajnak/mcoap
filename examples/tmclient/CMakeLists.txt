cmake_minimum_required(VERSION 3.2)
project(tmclient)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

set(SOURCE_FILES
    tmclient.c)

add_executable(tmclient ${SOURCE_FILES})
add_dependencies(tmclient msys mnet mcoap)
target_link_libraries(tmclient mcoap mnet msys pthread m)
