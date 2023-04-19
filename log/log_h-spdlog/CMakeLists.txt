cmake_minimum_required(VERSION 3.10)
project(spdlog_demo CXX)

option(USE_FIND_PACKAGE "use find_package(spdlog REQUIRED) to get spdlog" ON)

#使用find_package查找spdlog
if(USE_FIND_PACKAGE)

    #设置你的spdlog路径
    set(spdlog_DIR ${CMAKE_SOURCE_DIR}/../3rdparty/spdlog-1.9.2/lib64/cmake/spdlog)
    if(NOT TARGET spdlog)
        find_package(spdlog REQUIRED)
        message("FOUND spdlog : " ${spdlog_FOUND})
    endif()

else()#不使用find_package查找spdlog

    set(spdlog_DIR ${CMAKE_SOURCE_DIR}/../3rdparty/spdlog-1.9.2)
    include_directories(${spdlog_DIR}/include)
    link_directories(${spdlog_DIR}/lib64)

endif()

# 必须指定C++11 或指定 -std=c++0x
set(CMAKE_CXX_FLAGS "-std=c++0x")
# set(CMAKE_CXX_STANDARD 11)

add_definitions(-DUSE_SPDLOG)
add_definitions(-O3)

include_directories(${CMAKE_SOURCE_DIR})
file(GLOB SRC_DIR_LIST  *.cpp)

message(STATUS "SRC_DIR_LIST:\t[${SRC_DIR_LIST}]")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})

#只使用头文件编译
if(SPDLOG_BUILD_EXAMPLE_HO)

    if(USE_FIND_PACKAGE)
        add_executable(example_header_only ${SRC_DIR_LIST})
        target_link_libraries(example_header_only PRIVATE spdlog::spdlog_header_only)
    else()
        add_executable(example_header_only ${SRC_DIR_LIST})
    endif()

else()#链接库

    if(USE_FIND_PACKAGE)
        add_executable(example ${SRC_DIR_LIST})
        target_link_libraries(example PRIVATE spdlog::spdlog)
    else()
        add_definitions(-DSPDLOG_COMPILED_LIB)
        link_libraries(-lspdlog)
        add_executable(example ${SRC_DIR_LIST})
    endif()

endif()
