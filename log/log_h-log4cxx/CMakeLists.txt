cmake_minimum_required(VERSION 3.10)
project(log4cxx_demo CXX)

option(USE_NEW_LOG4CXX "ON: use log4cxx-0.11.0, OFF: log4cxx-0.9.7" ON)

if(USE_NEW_LOG4CXX)
    add_definitions(-DUSE_LOG4CXX)
    #设置你的log4cxx路径
    set(log4cxx_DIR ${CMAKE_SOURCE_DIR}/../3rdparty/logging-log4cxx-0.11.0)
else()
    add_definitions(-DUSE_LOG4CXX_0_9_7)
    #设置你的log4cxx路径
    set(log4cxx_DIR ${CMAKE_SOURCE_DIR}/../3rdparty/logging-log4cxx-0_9_7)
endif()

# cmake 开启GDB模式：
# SET(CMAKE_BUILD_TYPE "Debug")
# SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
# SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
# SET(CMAKE_CXX_FLAGS "-std=c++98")
add_definitions(-O3)

include_directories(${CMAKE_SOURCE_DIR}
                    ${log4cxx_DIR}/include)

link_directories(${log4cxx_DIR}/lib64)

file(GLOB SRC_DIR_LIST  ${CMAKE_SOURCE_DIR}/*.cpp)

message(STATUS "SRC_DIR_LIST:\t[${SRC_DIR_LIST}]")

link_libraries(-llog4cxx)
add_executable(example ${SRC_DIR_LIST})

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})
