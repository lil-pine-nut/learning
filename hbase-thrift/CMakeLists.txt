cmake_minimum_required (VERSION 2.9)

# 项目信息
project (Learn_Thrift)

option(Build_Thrift_0_10 "option for build thrift-0.10.0 - on: build thrift-0.10.0 ; off: build thrift-0.16.0" ON)
message("Build_Thrift_0_10 = " ${Build_Thrift_0_10})

set(BOOST_PATH ${CMAKE_SOURCE_DIR}/3rdparty/boost_1_64_0)


# O3编译优化
add_definitions(-O3)

# -fPIC
add_compile_options(-fPIC)

link_libraries(pthread)

add_subdirectory(demo)
add_subdirectory(Hbase-Client-Class)