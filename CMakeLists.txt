cmake_minimum_required(VERSION 3.3)
project(mcoap)

# Appears to be required to force compiling for C.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/build/")
set(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/build/")

include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(cutest)
add_subdirectory(msys)
add_subdirectory(testms)
add_subdirectory(mnet)
add_subdirectory(mcoap)
add_subdirectory(testmc)
add_subdirectory(mcget)
add_subdirectory(testbase)