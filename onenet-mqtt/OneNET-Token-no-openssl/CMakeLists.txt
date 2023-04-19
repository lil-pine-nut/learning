
# 选择include的目录
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/include )

# 设置执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

# 可执行文件生成,链接静态库
add_executable(token2 ${CMAKE_CURRENT_SOURCE_DIR}/Token-no-openssl.cpp ${CMAKE_CURRENT_SOURCE_DIR}/HMAC_SHA1.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/SHA1.cpp ${CMAKE_CURRENT_SOURCE_DIR}/ZBase64.cpp)