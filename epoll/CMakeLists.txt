cmake_minimum_required (VERSION 2.9)

# 项目信息
project (Learn_Epoll)

# O3编译优化
#add_definitions(-O3)

link_libraries(pthread)

add_executable(client client.cpp)
add_executable(server server.cpp)
add_executable(server-ptr server-ptr.cpp)
add_executable(epoll_event_fd epoll_event_fd.cpp)