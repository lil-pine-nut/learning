include_directories(${CMAKE_CURRENT_BINARY_DIR})
# message("${CMAKE_CURRENT_BINARY_DIR}")

file(GLOB cgi_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/api.cpp  ${CMAKE_CURRENT_BINARY_DIR}/soapC.cpp
                        ${gSOAP_SRC} ${gSOAP_PLUGIN}/httppost.c)
set(NAME "api")
# 把 "api.exe" 改名为 "api.cgi"
add_executable(${NAME} ${cgi_SOURCE})

file(GLOB myapp_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/myapp.cpp ${CMAKE_CURRENT_BINARY_DIR}/soapC.cpp ${gSOAP_SRC})
set(app_NAME "myapp_restApi")
add_executable(${app_NAME} ${myapp_SOURCE})