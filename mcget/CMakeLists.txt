cmake_minimum_required(VERSION 3.3)
project(mcget)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

set(SOURCE_FILES
    mcget.c)

add_executable(mcget ${SOURCE_FILES})
add_dependencies(mcget cutest msys mnet mcoap)
target_link_libraries(mcget mcoap mnet msys pthread m)