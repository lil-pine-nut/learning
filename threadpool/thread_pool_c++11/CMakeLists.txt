set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
link_libraries(-lpthread)
add_executable(test ${CMAKE_CURRENT_SOURCE_DIR} test.cpp)
target_compile_options(test PUBLIC -std=c++11)