set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
link_libraries(-lssl -lcrypto -lpthread)
add_executable(test_client client.c common.c sslthread.c)
add_executable(test_server server.c common.c sslthread.c)

