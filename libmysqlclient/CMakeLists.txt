cmake_minimum_required (VERSION 2.9)

# 项目信息
project(learn_libmysqlclient)

# 设置三方库的路径
set(LIBMYSQLCLIENT_PATH      ${CMAKE_SOURCE_DIR}/3rdparty/mysql)

# include三方库
include_directories(${LIBMYSQLCLIENT_PATH}/include)

link_directories(${LIBMYSQLCLIENT_PATH}/lib)

link_libraries(-lmysqlclient)

# # GDB调试
# SET(CMAKE_BUILD_TYPE "Debug")
# SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
# SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

# 指定c++版本
# set(CMAKE_CXX_STANDARD 11)

# -O3编译优化
# add_definitions("-O3")

include_directories(mysqlclient)

add_executable(test1 test1.cpp mysqlclient/mysqlclient.cpp)
add_executable(test2 test2.cpp mysqlclient/mysqlclient.cpp)