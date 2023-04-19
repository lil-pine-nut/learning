cmake_minimum_required (VERSION 2.9)

# 项目信息
project(learn_zeromq)

# 设置三方库的路径
set(ZEROMQ_PATH      ${CMAKE_SOURCE_DIR}/3rdparty/zeromq-4.3.4)

# include三方库
include_directories(${ZEROMQ_PATH}/include)

link_directories(${ZEROMQ_PATH}/lib)

link_libraries(-lzmq)

add_executable(pub ${CMAKE_SOURCE_DIR}/pub.cpp)
add_executable(sub ${CMAKE_SOURCE_DIR}/sub.cpp)


