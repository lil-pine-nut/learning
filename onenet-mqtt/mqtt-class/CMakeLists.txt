# 选择include的目录
INCLUDE_DIRECTORIES(${PAHO_MQTT_PATH}/include)

link_directories(${PAHO_MQTT_PATH}/lib64)

link_libraries(-lpaho-mqtt3a -lssl -lcrypto)

# 可执行文件生成,链接静态库
add_executable(subscribe ${CMAKE_CURRENT_SOURCE_DIR}/subscribe.cpp ${CMAKE_CURRENT_SOURCE_DIR}/MQTTCli.cpp ${CMAKE_CURRENT_SOURCE_DIR}/OneNET.cpp)
add_executable(publish ${CMAKE_CURRENT_SOURCE_DIR}/publish.cpp ${CMAKE_CURRENT_SOURCE_DIR}/MQTTCli.cpp ${CMAKE_CURRENT_SOURCE_DIR}/OneNET.cpp)