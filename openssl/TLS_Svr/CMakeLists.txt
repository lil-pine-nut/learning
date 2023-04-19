set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
link_libraries(-lssl -lcrypto)
add_executable(tls_server server.cpp TLS_Svr.cpp)

file(GLOB HAS_FILE ${CMAKE_CURRENT_BINARY_DIR}/server.crt)
if("${HAS_FILE}" STREQUAL "")    # 判断HAS_NSMAP是否为空
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/server.crt ${CMAKE_CURRENT_BINARY_DIR})
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/server.key ${CMAKE_CURRENT_BINARY_DIR})
endif()

