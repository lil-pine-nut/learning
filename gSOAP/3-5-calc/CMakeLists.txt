# GDB调试
# SET(CMAKE_BUILD_TYPE "Debug")
# SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
# SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
# message("${CMAKE_CURRENT_BINARY_DIR}")

add_definitions(-DCALC_INDEX_HTML="${CMAKE_CURRENT_SOURCE_DIR}/calc-index.html")
message("${CMAKE_CURRENT_SOURCE_DIR}/calc-index.html")

file(GLOB SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/calc-mul-thread-server.cpp ${CMAKE_CURRENT_SOURCE_DIR}/HttpService.cpp
                    ${CMAKE_CURRENT_BINARY_DIR}/soapC.cpp ${gSOAP_SRC} ${CMAKE_CURRENT_BINARY_DIR}/soapcalcService.cpp ${gSOAP_PLUGIN}/httpform.c)
set(NAME "cal-5-mul-thread-server")
add_executable(${NAME} ${SOURCE})
target_link_libraries(${NAME} -lpthread)

file(GLOB myapp_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/calc-client.cpp ${CMAKE_CURRENT_BINARY_DIR}/soapC.cpp 
                    ${gSOAP_SRC} ${CMAKE_CURRENT_BINARY_DIR}/soapcalcProxy.cpp)
set(app_NAME "cal-5-client")
add_executable(${app_NAME} ${myapp_SOURCE})
