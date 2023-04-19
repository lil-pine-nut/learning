cmake_minimum_required (VERSION 2.9)

# 项目信息
project(learn_libssh2)

# 设置三方库的路径
set(LIBSSH2_PATH    ${CMAKE_SOURCE_DIR}/3rdparty/libssh2-1.10.0)

# include三方库
include_directories(${LIBSSH2_PATH}/include)

link_directories(${LIBSSH2_PATH}/lib)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})

# # GDB调试
# SET(CMAKE_BUILD_TYPE "Debug")
# SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
# SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

# set(CMAKE_CXX_STANDARD 11)

add_definitions("-O3")

link_libraries( -lssh2 )

add_executable(sftpdir ${CMAKE_SOURCE_DIR}/example/sftpdir.c)