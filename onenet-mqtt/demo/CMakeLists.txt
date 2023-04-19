# 选择include的目录
INCLUDE_DIRECTORIES(${PAHO_MQTT_PATH}/include)

link_directories(${PAHO_MQTT_PATH}/lib64)

link_libraries(-lpaho-mqtt3c)

# 可执行文件生成,链接静态库
add_executable(sub-pub ${CMAKE_CURRENT_SOURCE_DIR}/sub-pub.cpp)