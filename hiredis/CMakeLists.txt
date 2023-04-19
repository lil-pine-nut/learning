# 指定CMake编译最低要求版本
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

# 给项目命名
PROJECT(learn-hiredis)

# # GDB调试
# SET(CMAKE_BUILD_TYPE "Debug")
# SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
# SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

# 设置三方库的路径
set(HIREDIS_PATH ${CMAKE_SOURCE_DIR}/3rdparty/hiredis)

# 选择include的目录
include_directories(${HIREDIS_PATH}/include)

# 查找so
file(GLOB CURL_LIB ${HIREDIS_PATH}/lib/*.a)

add_executable(demo demo.cpp)
target_link_libraries(demo ${CURL_LIB})

