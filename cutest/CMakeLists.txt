cmake_minimum_required(VERSION 3.3)
project(cutest)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

set(SOURCE_FILES
    AllTests.c
    CuTest.c
    CuTest.h
    CuTestTest.c)

add_library(cutest ${SOURCE_FILES})