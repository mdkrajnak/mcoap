cmake_minimum_required(VERSION 3.2)
project(mnet)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

add_library(mnet  
    mn_error.c
    mn_error.h
    mn_sockaddr.c
    mn_sockaddr.h
    mn_socket.h
    mn_timeout.c
    mn_timeout.h)

if (WIN32)
    target_sources(mnet PRIVATE mn_socket_win.h mn_socket_win.c)
    add_definitions(mnet -D_WINSOCK_DEPRECATED_NO_WARNINGS)
else()
    target_sources(mnet PRIVATE mn_socket_unix.h mn_socket_unix.c)
endif()

