cmake_minimum_required(VERSION 3.2)
project(examples)

# Appears to be required to force compiling for C.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

add_subdirectory(tmserver)
add_subdirectory(tmclient)

