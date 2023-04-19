# 设置执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

# 可执行文件生成,链接静态库
add_executable(sftp-url-PersistentList ${CMAKE_CURRENT_SOURCE_DIR}/demo.cpp)

target_link_libraries(sftp-url-PersistentList  ${CURL_LIB} pthread)