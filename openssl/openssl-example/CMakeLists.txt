set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
link_libraries(-lssl -lcrypto)
add_executable(async-ssl-cli async-ssl-cli.cc)
add_executable(async-ssl-svr async-ssl-svr.cc)
add_executable(sync-ssl-cli  sync-ssl-cli.cc)
add_executable(sync-ssl-svr  sync-ssl-svr.cc)

file(GLOB HAS_FILE ${CMAKE_CURRENT_BINARY_DIR}/server.pem)
if("${HAS_FILE}" STREQUAL "")    # 判断HAS_NSMAP是否为空
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/server.pem ${CMAKE_CURRENT_BINARY_DIR})
endif()

