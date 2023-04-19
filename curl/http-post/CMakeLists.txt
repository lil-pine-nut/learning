# GDB调试
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

# 设置执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

# 可执行文件生成,链接静态库
add_executable(http-post ${CMAKE_CURRENT_SOURCE_DIR}/http-post.cpp)
add_executable(http-post-2 ${CMAKE_CURRENT_SOURCE_DIR}/http-post-2.cpp)

target_link_libraries(http-post ${CURL_LIB})
target_compile_definitions(http-post PUBLIC
-DCurl_Version_TXT="${CMAKE_CURRENT_SOURCE_DIR}/test-curl_version.txt")

target_link_libraries(http-post-2 ${CURL_LIB})
target_compile_definitions(http-post-2 PUBLIC
-DCurl_Version_TXT="${CMAKE_CURRENT_SOURCE_DIR}/test-curl_version.txt")