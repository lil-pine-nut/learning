# 设置三方库的路径
set(ONENET_STUDIO_SDK_PATH ${CMAKE_SOURCE_DIR}/modify_lib/onenet_studio_sdk_20210309/install)

# 选择include的目录
INCLUDE_DIRECTORIES(${ONENET_STUDIO_SDK_PATH}/include)

link_directories(${ONENET_STUDIO_SDK_PATH}/lib)

link_libraries(-laio_platforms -laio_onenet_tm -laio_tools_cjson -laio_onenet_utils -laio_er_coap_13 -laio_paho_mqtt -laio_http -laio_security -laio_tools_wolfssl -lota_core -laio_common -lota_adapter -lota_http_parse -laio_platforms)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/3rdparty/cJSON)

# 生成可执行文件
add_executable(demo1 ${CMAKE_CURRENT_SOURCE_DIR}/main.c ${CMAKE_CURRENT_SOURCE_DIR}/tm_user.c)
add_executable(demo2 ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
