# 生成可执行文件 test_pack_json
add_executable(test_pack_json ${CMAKE_CURRENT_SOURCE_DIR}/test_pack_json.cpp ${CMAKE_CURRENT_SOURCE_DIR}/onejson.cpp ${CMAKE_SOURCE_DIR}/3rdparty/cJSON/cJSON.c)

# test_pack_json 头文件目录
target_include_directories(test_pack_json PUBLIC
${CMAKE_CURRENT_SOURCE_DIR}
${CMAKE_SOURCE_DIR}/3rdparty/cJSON
)