cmake_minimum_required (VERSION 2.9)

# 项目信息
project(learn_network)

# 设置三方库的路径
set(LIBUV_PATH      ${CMAKE_SOURCE_DIR}/3rdparty/libuv-1.43.0)
set(HTTP_PASER_PATH ${CMAKE_SOURCE_DIR}/3rdparty/http-parser-2.9.4)

# include三方库
include_directories(${LIBUV_PATH}/include
                    ${HTTP_PASER_PATH})

link_directories(${LIBUV_PATH}/lib64)

# GDB调试
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

# MarkdownTransform所需
set(CMAKE_CXX_STANDARD 11)
# add_definitions("-O3")

link_libraries( -luv_a -lpthread -ldl)

# https://github.com/springmeyer/libuv-webserver
add_executable(libuv-webserver ${CMAKE_SOURCE_DIR}/libuv-webserver.cpp
                ${HTTP_PASER_PATH}/http_parser.c)

include_directories(${CMAKE_SOURCE_DIR}/OpenSSL_Server)
link_libraries(-lssl -lcrypto)
set(SERVER_CRT ${CMAKE_SOURCE_DIR}/OpenSSL_Server/cert.pem)
set(SERVER_KEY ${CMAKE_SOURCE_DIR}/OpenSSL_Server/key.pem)
add_definitions(-DSERVER_CRT="${SERVER_CRT}")
add_definitions(-DSERVER_KEY="${SERVER_KEY}")

# add_definitions(-DDEBUG)
# message("CRT file: "  ${SERVER_CRT})
# message("CRT file: "  ${SERVER_KEY})
# https://github.com/springmeyer/libuv-webserver
add_executable(libuv-webserver-openssl ${CMAKE_SOURCE_DIR}/libuv-webserver-openssl.cpp
                ${HTTP_PASER_PATH}/http_parser.c ${CMAKE_SOURCE_DIR}/OpenSSL_Server/OpenSSL_Server.cpp)

add_executable(http-mp4 ${CMAKE_SOURCE_DIR}/http-mp4.cpp)

add_executable(libuv-queue-demo ${CMAKE_SOURCE_DIR}/libuv-queue-demo.cpp)