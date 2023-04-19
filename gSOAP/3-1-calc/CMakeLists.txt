include_directories(${CMAKE_CURRENT_BINARY_DIR})
message("${CMAKE_CURRENT_BINARY_DIR}")

file(GLOB SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/calc-server.cpp  ${CMAKE_CURRENT_BINARY_DIR}/soapC.cpp 
                    ${gSOAP_SRC} ${CMAKE_CURRENT_BINARY_DIR}/soapServer.cpp)
set(NAME "cal-1-server")
add_executable(${NAME} ${SOURCE})

file(GLOB myapp_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/calc-client.cpp ${CMAKE_CURRENT_BINARY_DIR}/soapC.cpp 
                    ${gSOAP_SRC} ${CMAKE_CURRENT_BINARY_DIR}/soapClient.cpp)
set(app_NAME "cal-1-client")
add_executable(${app_NAME} ${myapp_SOURCE})