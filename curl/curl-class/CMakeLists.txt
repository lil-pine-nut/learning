# 选择include的目录
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/include )

# 设置执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

# 可执行文件生成,链接静态库
add_executable(test_client ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp ${CMAKE_CURRENT_SOURCE_DIR}/curl-client.cpp)

target_link_libraries(test_client  ${CURL_LIB})