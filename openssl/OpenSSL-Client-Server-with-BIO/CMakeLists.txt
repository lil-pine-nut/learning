set(OPENSSL_PATH ${CMAKE_SOURCE_DIR}/3rdparty/openssl-1.1.1q)
include_directories(${OPENSSL_PATH}/include)
link_directories(${OPENSSL_PATH}/lib)
link_libraries(-lssl -lcrypto)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
add_executable(client ${CMAKE_CURRENT_SOURCE_DIR}/Client/Main.cpp ${CMAKE_CURRENT_SOURCE_DIR}/Client/OpenSSL_BIO_Client.cpp)
add_executable(server ${CMAKE_CURRENT_SOURCE_DIR}/Server/Main.cpp ${CMAKE_CURRENT_SOURCE_DIR}/Server/OpenSSL_BIO_Server.cpp)

file(GLOB HAS_FILE ${CMAKE_CURRENT_BINARY_DIR}/cert.pem)
if("${HAS_FILE}" STREQUAL "")    # 判断HAS_NSMAP是否为空
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/cert.pem ${CMAKE_CURRENT_BINARY_DIR})
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/key.pem ${CMAKE_CURRENT_BINARY_DIR})
endif()

