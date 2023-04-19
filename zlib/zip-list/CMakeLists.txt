set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
add_executable(unzip-gunzip unzip-gunzip.cpp ioapi.c unzip.c)
add_executable(zip-list zip-list.cpp ioapi.c unzip.c)