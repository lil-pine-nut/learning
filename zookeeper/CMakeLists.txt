cmake_minimum_required (VERSION 2.9)

# 项目信息
project(learn_zookeeper)

# 设置三方库的路径
set(ZOOKEEPER_PATH      ${CMAKE_SOURCE_DIR}/3rdparty/apache-zookeeper-3.6.4)

# include三方库
include_directories(${ZOOKEEPER_PATH}/include
                    ${ZOOKEEPER_PATH}/include/zookeeper)

link_directories(${ZOOKEEPER_PATH}/lib)

link_libraries(-lzookeeper)

# # GDB调试
# SET(CMAKE_BUILD_TYPE "Debug")
# SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
# SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

# 指定c++版本
# set(CMAKE_CXX_STANDARD 11)

# -O3编译优化
# add_definitions("-O3")


add_executable(test_init ${CMAKE_SOURCE_DIR}/test_init/zkClient.cpp)

add_executable(QueryServer ${CMAKE_SOURCE_DIR}/QueryServer/QueryServer.cpp)
add_executable(QueryServerd ${CMAKE_SOURCE_DIR}/QueryServer/QueryServerd.cpp)

add_executable(get_value ${CMAKE_SOURCE_DIR}/get_value/demo.cpp)

add_executable(config_mg ${CMAKE_SOURCE_DIR}/Configuration_Management/config_mg.cpp)
