# 生成可执行文件 test_pack_json
add_executable(cjson_demo ${CMAKE_CURRENT_SOURCE_DIR}/cjson_demo.cpp ${CMAKE_SOURCE_DIR}/3rdparty/cJSON/cJSON.c)

# test_pack_json 头文件目录
target_include_directories(cjson_demo PUBLIC
${CMAKE_CURRENT_SOURCE_DIR}
${CMAKE_SOURCE_DIR}/3rdparty/cJSON
)