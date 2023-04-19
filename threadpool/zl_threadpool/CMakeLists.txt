set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
link_libraries(-lpthread)
add_executable(ThreadPoolCpp03 ${CMAKE_CURRENT_SOURCE_DIR}/ThreadPoolCpp03/main.cpp ${CMAKE_CURRENT_SOURCE_DIR}/ThreadPoolCpp03/ThreadPool.cpp)
target_compile_options(ThreadPoolCpp03 PUBLIC -std=c++0x)

add_executable(ThreadPoolCpp11 ${CMAKE_CURRENT_SOURCE_DIR}/ThreadPoolCpp11/main.cpp ${CMAKE_CURRENT_SOURCE_DIR}/ThreadPoolCpp11/ThreadPool.cpp)
target_compile_options(ThreadPoolCpp11 PUBLIC -std=c++11)

add_executable(ThreadPoolCpp98 ${CMAKE_CURRENT_SOURCE_DIR}/ThreadPoolCpp98/main.cpp ${CMAKE_CURRENT_SOURCE_DIR}/ThreadPoolCpp98/ThreadPool.cpp)
target_compile_options(ThreadPoolCpp98 PUBLIC -std=c++98)