# 指定CMake编译最低要求版本
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

# 给项目命名
PROJECT(learn-curl)

# # GDB调试
# SET(CMAKE_BUILD_TYPE "Debug")
# SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
# SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

# 设置三方库的路径
set(CURL_PATH ${CMAKE_SOURCE_DIR}/3rdparty/curl-7.82.0)

# 选择include的目录
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}
                    ${CURL_PATH}/include
)

# 查找so
file(GLOB CURL_LIB ${CURL_PATH}/lib/*.so*)



add_subdirectory(example/ftp)
add_subdirectory(example/sftp)
add_subdirectory(big-file-curl)
add_subdirectory(curl-class)
add_subdirectory(sftp-url-PersistentList)
add_subdirectory(http-get)
add_subdirectory(http-post)


