cmake_minimum_required (VERSION 2.9)

# 项目信息
project (Learn_modernCpp)

# O3编译优化
#add_definitions(-O3)

# 指定std版本
set(CMAKE_CXX_STANDARD 11)

add_subdirectory(atomic)
add_subdirectory(condition_variable)
add_subdirectory(emplace_back)
add_subdirectory(poller)