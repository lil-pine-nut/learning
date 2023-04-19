cmake_minimum_required (VERSION 2.9)

# 项目信息
project(learn_librdkafka)

# 设置三方库的路径
set(LIBRDKAFKA_PATH      ${CMAKE_SOURCE_DIR}/3rdparty/librdkafka-1.6.2)

# include三方库
include_directories(${LIBRDKAFKA_PATH}/include
                    ${LIBRDKAFKA_PATH}/include/librdkafka)

link_directories(${LIBRDKAFKA_PATH}/lib)

link_libraries(-lrdkafka++ -lrdkafka)

# # GDB调试
# SET(CMAKE_BUILD_TYPE "Debug")
# SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
# SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

# 指定c++版本
# set(CMAKE_CXX_STANDARD 11)

# -O3编译优化
# add_definitions("-O3")


add_executable(example-KafkaProducer ${CMAKE_SOURCE_DIR}/example/example-Producer.cpp)
add_executable(example-KafkaConsumer ${CMAKE_SOURCE_DIR}/example/example-Consumer.cpp)


include_directories(${CMAKE_SOURCE_DIR}/class)
add_executable(class-KafkaProducer ${CMAKE_SOURCE_DIR}/class/Producer-demo.cpp ${CMAKE_SOURCE_DIR}/class/KafkaProducer.cpp)
add_executable(class-KafkaConsumer ${CMAKE_SOURCE_DIR}/class/Consumer-demo.cpp ${CMAKE_SOURCE_DIR}/class/KafkaConsumer.cpp)

