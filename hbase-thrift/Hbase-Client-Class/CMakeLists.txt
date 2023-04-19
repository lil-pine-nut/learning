
if(${Build_Thrift_0_10})
    add_definitions(-O3)
    message("Hbase-Client-Class - Build_Thrift_0_10")
    set(Thrift_0_10_PATH ${CMAKE_SOURCE_DIR}/3rdparty/thrift-0.10.0)
    file(GLOB Thrift_0_10_LIBS ${Thrift_0_10_PATH}/lib/*.a)
    set(Thrift_0_10_Thrift_Gen_CPP_PATH  ${CMAKE_SOURCE_DIR}/3rdparty/hbase-thrift-gen-cpp/thrift-0.10.0-thrift-gen-cpp-hbase-1.3.0-src)
    file(GLOB Thrift_0_10_Thrift_Gen_CPP_SRC ${Thrift_0_10_Thrift_Gen_CPP_PATH}/Hbase_types.cpp
                                        ${Thrift_0_10_Thrift_Gen_CPP_PATH}/Hbase.cpp)

    message("Thrift 0.10.0 Thrift src = " ${Thrift_0_10_Thrift_Gen_CPP_SRC})

    #hbase-thrift-master
    set(Hbase_Thrift_Class_Path ${CMAKE_SOURCE_DIR}/Hbase-Client-Class)
    include_directories(    ${BOOST_PATH}/include
                            ${Thrift_0_10_PATH}/include
                            ${Thrift_0_10_PATH}/include/thrift
                            ${Thrift_0_10_Thrift_Gen_CPP_PATH}
                            ${Hbase_Thrift_Class_Path})

    add_executable(Hbase_Thrift_DEMO2    ${Hbase_Thrift_Class_Path}/hbcli.cpp  ${Hbase_Thrift_Class_Path}/demo.cpp
                                            ${Thrift_0_10_Thrift_Gen_CPP_SRC})
    target_link_libraries(Hbase_Thrift_DEMO2 ${Thrift_0_10_LIBS})

    add_executable(testput    ${Hbase_Thrift_Class_Path}/hbcli.cpp  ${Hbase_Thrift_Class_Path}/testput.cpp
                                            ${Thrift_0_10_Thrift_Gen_CPP_SRC})
    target_link_libraries(testput ${Thrift_0_10_LIBS})
endif()