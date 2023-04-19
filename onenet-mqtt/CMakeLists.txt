# 指定CMake编译最低要求版本
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

# 给项目命名
PROJECT(onenet-paho-mqtt-c)

# GDB调试
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

# 设置三方库的路径
set(PAHO_MQTT_PATH ${CMAKE_SOURCE_DIR}/3rdparty/paho.mqtt.c-1.3.10)


# 进入子目录
add_subdirectory(demo)
add_subdirectory(OneNET-Token-openssl)
add_subdirectory(OneNET-Token-no-openssl)
add_subdirectory(mqtt-class)
add_subdirectory(demo_onenet_studio_sdk)
add_subdirectory(gateway_sub-device)
add_subdirectory(onejson)
add_subdirectory(cjson_demo)