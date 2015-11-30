cmake_minimum_required(VERSION 3.3)
project(mnet)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

if (WIN32)
    set(SOCKET_IMPL "mn_socket_win32.c")
	add_definitions(mnet -D_WINSOCK_DEPRECATED_NO_WARNINGS)
elseif(!WIN32)
    set(SOCKET_IMPL "mn_socket_unix.c")
endif(WIN32)

# mn_socket_win32.c
# mn_socket_win32.h

set(SOURCE_FILES
    mn_error.c
    mn_error.h
    mn_sockaddr.c
    mn_sockaddr.h
    mn_socket.h
    mn_timeout.c
    mn_timeout.h
    ${SOCKET_IMPL})

add_library(mnet ${SOURCE_FILES})