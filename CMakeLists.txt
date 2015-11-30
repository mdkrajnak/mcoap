cmake_minimum_required(VERSION 3.3)
project(mcoap)

# Appears to be required to force compiling for C.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/build/")
set(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/build/")

include_directories(${PROJECT_SOURCE_DIR})

if (WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -DWIN32_LEAN_AND_MEAN)
endif(WIN32)

add_subdirectory(cutest)
add_subdirectory(msys)
add_subdirectory(testms)
add_subdirectory(mnet)
add_subdirectory(mcoap)
add_subdirectory(testmc)
add_subdirectory(mcget)
add_subdirectory(testbase)