# 生成可执行文件
add_executable(sub-device ${CMAKE_CURRENT_SOURCE_DIR}/sub-device.cpp)

# 设置三方库的路径
set(ONENET_STUDIO_SDK_PATH ${CMAKE_SOURCE_DIR}/3rdparty/onenet_studio_sdk_20210309)
set(CJSON_PATH ${CMAKE_SOURCE_DIR}/3rdparty/cJSON)

# 选择include的目录
INCLUDE_DIRECTORIES(${ONENET_STUDIO_SDK_PATH}/include)
INCLUDE_DIRECTORIES(${CJSON_PATH})

link_directories(${ONENET_STUDIO_SDK_PATH}/lib)

link_libraries(-laio_platforms -laio_onenet_tm -laio_tools_cjson -laio_onenet_utils -laio_er_coap_13 -laio_paho_mqtt -laio_http -laio_security -laio_tools_wolfssl -lota_core -laio_common -lota_adapter -lota_http_parse -laio_platforms)

# 生成可执行文件
add_executable(onenet-gateway ${CMAKE_CURRENT_SOURCE_DIR}/onenet-gateway.cpp ${CJSON_PATH}/cJSON.c)
