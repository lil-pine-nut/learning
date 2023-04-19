# 指定CMake编译最低要求版本
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

# 给项目命名
PROJECT(net-snmp)

# GDB调试
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

# 指定std版本
# set(CMAKE_CXX_STANDARD 11)

# -O3编译优化
# add_definitions(-O3)

# 取消警告
add_definitions(-w)

# 设置三方库的路径
set(NET_SNMP_PATH   ${CMAKE_SOURCE_DIR}/3rdparty/net-snmp-5.9.3)

# include三方库
include_directories(${NET_SNMP_PATH}/include
)

link_directories(${NET_SNMP_PATH}/lib)

link_libraries(-lnetsnmp)

# 设置执行文件输出路径
# set(EXECUTABLE_OUTPUT_PATH  ${CMAKE_CURRENT_BINARY_DIR})

add_executable(snmpdemoapp      src/snmpdemoapp.c)
add_executable(asyncapp         src/asyncapp.c)
add_executable(demo_snmpwalk    src/demo_snmpwalk.c)
add_executable(snmpwalk         src/snmpwalk.c)
add_executable(demo             src/demo.cpp)
add_executable(demo2            src/demo2.cpp)

add_executable(classdemo        class/classdemo.cpp class/SnmpManager.cpp class/SnmpClient.cpp)
target_include_directories(classdemo PUBLIC ${CMAKE_SOURCE_DIR}/class)