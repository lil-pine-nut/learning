if(${Build_Thrift_0_10})
    message("build thrift-0.10.0")
    set(Thrift_PATH ${CMAKE_SOURCE_DIR}/3rdparty/thrift-0.10.0)
    set(Thrift_Thrift_Gen_CPP_PATH  ${CMAKE_SOURCE_DIR}/3rdparty/hbase-thrift-gen-cpp/thrift-0.10.0-thrift-gen-cpp-hbase-1.3.0-src)
    set(Thrift_Thrift2_Gen_CPP_PATH ${CMAKE_SOURCE_DIR}/3rdparty/hbase-thrift-gen-cpp/thrift-0.10.0-thrift2-gen-cpp-hbase-1.3.0-src)
    file(GLOB Thrift_Thrift_Gen_CPP_SRC ${Thrift_Thrift_Gen_CPP_PATH}/Hbase_types.cpp
                                        ${Thrift_Thrift_Gen_CPP_PATH}/Hbase.cpp)
    file(GLOB Thrift_Thrift2_Gen_CPP_SRC    ${Thrift_Thrift2_Gen_CPP_PATH}/hbase_types.cpp
                                            ${Thrift_Thrift2_Gen_CPP_PATH}/THBaseService.cpp)
else()

    message("build thrift-0.16.0")
    # 指定std版本
    set(CMAKE_CXX_STANDARD 11)
    set(Thrift_PATH ${CMAKE_SOURCE_DIR}/3rdparty/thrift-0.16.0)
    set(Thrift_Thrift_Gen_CPP_PATH ${CMAKE_SOURCE_DIR}/3rdparty/hbase-thrift-gen-cpp/thrift-0.16.0-thrift-gen-cpp-hbase-1.3.0-src)
    set(Thrift_Thrift2_Gen_CPP_PATH ${CMAKE_SOURCE_DIR}/3rdparty/hbase-thrift-gen-cpp/thrift-0.16.0-thrift2-gen-cpp-hbase-1.3.0-src)
    file(GLOB Thrift_Thrift_Gen_CPP_SRC ${Thrift_Thrift_Gen_CPP_PATH}/Hbase_types.cpp
                                        ${Thrift_Thrift_Gen_CPP_PATH}/Hbase.cpp)
    file(GLOB Thrift_Thrift2_Gen_CPP_SRC    ${Thrift_Thrift2_Gen_CPP_PATH}/hbase_types.cpp
                                            ${Thrift_Thrift2_Gen_CPP_PATH}/THBaseService.cpp)
endif()

# include三方库
include_directories(${Thrift_PATH}/include
                    ${Thrift_PATH}/include/thrift
                    ${Thrift_Thrift_Gen_CPP_PATH}
                    ${Thrift_Thrift2_Gen_CPP_PATH}
                    ${BOOST_PATH}/include
                    )



#
file(GLOB Thrift_LIBS ${Thrift_PATH}/lib/*.a)
message("Thrift path = " ${Thrift_PATH})
message("Thrift libs = " ${Thrift_LIBS})

message("Thrift Thrift src = " ${Thrift_Thrift_Gen_CPP_SRC})
message("Thrift Thrift2 src = " ${Thrift_Thrift2_Gen_CPP_SRC})

if(${Build_Thrift_0_10})
add_executable(${CMAKE_PROJECT_NAME}_Thrift     ${CMAKE_CURRENT_SOURCE_DIR}/thrift-0.10.0-demo.cpp
                                                ${Thrift_Thrift_Gen_CPP_SRC})
target_link_libraries(${CMAKE_PROJECT_NAME}_Thrift ${Thrift_LIBS})

add_executable(${CMAKE_PROJECT_NAME}_Thrift_2   ${CMAKE_CURRENT_SOURCE_DIR}/thrift-0.10.0-demo2.cpp
                                                ${Thrift_Thrift2_Gen_CPP_SRC})
target_link_libraries(${CMAKE_PROJECT_NAME}_Thrift_2 ${Thrift_LIBS})

else()

add_executable(${CMAKE_PROJECT_NAME}_Thrift2     ${CMAKE_CURRENT_SOURCE_DIR}/thrift-0.16.0-demo.cpp
                                                ${Thrift_Thrift_Gen_CPP_SRC})
target_link_libraries(${CMAKE_PROJECT_NAME}_Thrift2 ${Thrift_LIBS})

add_executable(${CMAKE_PROJECT_NAME}_Thrift2_2    ${CMAKE_CURRENT_SOURCE_DIR}/thrift-0.16.0-demo2.cpp
                                                ${Thrift_Thrift2_Gen_CPP_SRC})
target_link_libraries(${CMAKE_PROJECT_NAME}_Thrift2_2 ${Thrift_LIBS})

endif()