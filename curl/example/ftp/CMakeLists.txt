# 设置执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

# 可执行文件生成,链接静态库
add_executable(ftptestupload ${CMAKE_CURRENT_SOURCE_DIR}/ftptestupload.cpp)

target_link_libraries(ftptestupload  ${CURL_LIB})

# 可执行文件生成,链接静态库
add_executable(ftptestdownload ${CMAKE_CURRENT_SOURCE_DIR}/ftptestdownload.cpp)

target_link_libraries(ftptestdownload  ${CURL_LIB})

# 可执行文件生成,链接静态库
add_executable(ftp_client ${CMAKE_CURRENT_SOURCE_DIR}/ftp_client.cpp)

target_link_libraries(ftp_client  ${CURL_LIB})