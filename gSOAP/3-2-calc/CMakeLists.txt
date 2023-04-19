include_directories(${CMAKE_CURRENT_BINARY_DIR})
# message("${CMAKE_CURRENT_BINARY_DIR}")

add_definitions(-DCALC_WSDL_PATH="${CMAKE_CURRENT_BINARY_DIR}/calc.wsdl")

file(GLOB SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/calc-server.cpp  ${CMAKE_CURRENT_BINARY_DIR}/soapC.cpp 
                    ${gSOAP_SRC} ${CMAKE_CURRENT_BINARY_DIR}/soapcalcService.cpp)
set(NAME "cal-2-server")
add_executable(${NAME} ${SOURCE})

file(GLOB myapp_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/calc-client.cpp ${CMAKE_CURRENT_BINARY_DIR}/soapC.cpp 
                    ${gSOAP_SRC} ${CMAKE_CURRENT_BINARY_DIR}/soapcalcProxy.cpp)
set(app_NAME "cal-2-client")
add_executable(${app_NAME} ${myapp_SOURCE})
