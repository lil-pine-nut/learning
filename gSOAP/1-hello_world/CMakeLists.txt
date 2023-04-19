
# 拷贝文件
# execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${gSOAP_SRC} ${CMAKE_CURRENT_BINARY_DIR})
# execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${gSOAP_include} ${CMAKE_CURRENT_BINARY_DIR})

# 执行g++命令
# execute_process(COMMAND g++ -o ${CMAKE_CURRENT_BINARY_DIR}/hello.cgi ${CMAKE_CURRENT_BINARY_DIR}/hello.cpp ${CMAKE_CURRENT_BINARY_DIR}/soapC.cpp ${CMAKE_CURRENT_BINARY_DIR}/soapServer.cpp ${gSOAP_SRC} -I${gSOAP_PATH}/include/)
# execute_process(COMMAND g++ -o ${CMAKE_CURRENT_BINARY_DIR}/myapp ${CMAKE_CURRENT_BINARY_DIR}/myapp.cpp ${CMAKE_CURRENT_BINARY_DIR}/soapC.cpp ${CMAKE_CURRENT_BINARY_DIR}/soapClient.cpp ${gSOAP_SRC} -I${gSOAP_PATH}/include/)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
# message("${CMAKE_CURRENT_BINARY_DIR}")

file(GLOB cgi_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/hello.cpp ${CMAKE_CURRENT_BINARY_DIR}/soapC.cpp ${CMAKE_CURRENT_BINARY_DIR}/soapServer.cpp ${gSOAP_SRC})
set(cgi_NAME "hello")
# 把 "hello.exe" 改名为 "hello.cgi"
add_executable(${cgi_NAME} ${cgi_SOURCE})

file(GLOB myapp_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/myapp.cpp ${CMAKE_CURRENT_BINARY_DIR}/soapC.cpp ${CMAKE_CURRENT_BINARY_DIR}/soapClient.cpp ${gSOAP_SRC})
set(NAME "myapp")
# message("app_SOURCE = " ${myapp_SOURCE})
add_executable(${NAME} ${myapp_SOURCE})