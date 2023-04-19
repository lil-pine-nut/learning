cmake_minimum_required(VERSION 3.10)
project(log4cplus_demo CXX)


# cmake 开启GDB模式：
# SET(CMAKE_BUILD_TYPE "Debug")
# SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
# SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
# SET(CMAKE_CXX_FLAGS "-std=c++98")
add_definitions(-O3)

#设置你的log4cplus路径
set(log4cplus_DIR ${CMAKE_SOURCE_DIR}/../3rdparty/log4cplus-1.2.2)
include_directories(${CMAKE_SOURCE_DIR}
                    ${log4cplus_DIR}/include)

link_directories(${log4cplus_DIR}/lib)

file(GLOB SRC_DIR_LIST  "*.cpp"
                        )

message(STATUS "SRC_DIR_LIST:\t[${SRC_DIR_LIST}]")

link_libraries(-llog4cplus)
add_executable(example ${SRC_DIR_LIST})

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})
