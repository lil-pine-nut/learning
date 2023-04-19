set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
link_libraries(pthread)
add_executable(poller test.cpp Poller.cpp)