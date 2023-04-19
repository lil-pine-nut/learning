cmake_minimum_required (VERSION 2.9)

# 项目信息
project (Learn_izip)

# O3编译优化
#add_definitions(-O3)

link_libraries(z)

add_subdirectory(gzip)
add_subdirectory(untgz)
add_subdirectory(zip-list)