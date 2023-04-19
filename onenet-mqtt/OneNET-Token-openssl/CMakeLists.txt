
link_libraries(-lssl -lcrypto)

# 可执行文件生成,链接静态库
add_executable(token ${CMAKE_CURRENT_SOURCE_DIR}/token.cpp)