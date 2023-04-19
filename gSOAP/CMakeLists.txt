cmake_minimum_required (VERSION 2.8)

# 项目信息
project (Learn_gSOAP)

# 取消警告
add_definitions(-w)

# -O3编译优化
add_definitions(-O3)

# 设置三方库的路径
set(gSOAP_PATH ${CMAKE_SOURCE_DIR}/thirdparty/gsoap-2.8)
set(gSOAP_PLUGIN ${CMAKE_SOURCE_DIR}/thirdparty/gsoap-2.8/plugin)

# windows下使用cygwin编译
# 使用 cmake -G "Unix Makefiles" .. 选择cygwin的编译器
# set(CMAKE_BUILD_TYPE "Debug") # "Release" or "Debug" 
# 直接使用 cmake -G "Unix Makefiles" .. 编译的是release版本
if(WIN32)
    set(gSOAP_BIN ${CMAKE_SOURCE_DIR}/thirdparty/gsoap-2.8/bin/soapcpp2.exe)
    set(wsdl2h_BIN ${CMAKE_SOURCE_DIR}/thirdparty/gsoap-2.8/bin/wsdl2h.exe)
    # 使用MinGW编译: cmake -G "MinGW Makefiles" .. 
    if(${CMAKE_GENERATOR} STREQUAL "MinGW Makefiles")
    link_libraries(-lwsock32)
    endif()
elseif(UNIX AND NOT APPLE)
    set(gSOAP_BIN ${CMAKE_SOURCE_DIR}/thirdparty/gsoap-2.8/bin/soapcpp2)
    set(wsdl2h_BIN ${CMAKE_SOURCE_DIR}/thirdparty/gsoap-2.8/bin/wsdl2h)
endif(WIN32)

# add_definitions(WITH_NONAMESPACES)
# add_definitions(-DWITH_NONAMESPACES)

# include三方库
INCLUDE_DIRECTORIES(${gSOAP_PATH})
INCLUDE_DIRECTORIES(${gSOAP_PATH}/include)
INCLUDE_DIRECTORIES(${gSOAP_PATH}/share/gsoap/)
INCLUDE_DIRECTORIES(${gSOAP_PLUGIN})

# gSOAP的源文件和头文件
# 本人于windows+cygwin编译gsoap的库总是存在问题......
aux_source_directory(${gSOAP_PATH}/src gSOAP_SRC)
file(GLOB gSOAP_include ${gSOAP_PATH}/include/*.h)

# 本次测试直接使用源文件进行编译, 如果有闲进行 windows+cygwin编译gsoap 使用库参考下：
# ./configure --prefix=/cygdrive/e/ChromeDownload/gsoap-2.8 后更改同configure目录下的Makefile文件, 
# 修改 CFLAGS 和 CXXFLAGS 如下: 
# CFLAGS = -g -O2 -DWITH_NONAMESPACES -DWITH_NO_C_LOCALE
# CXXFLAGS = -g -O2 -DWITH_NONAMESPACES -DWITH_NO_C_LOCALE
# 使用库文件
# file(GLOB gSOAP_LIB ${gSOAP_PATH}/lib/*.a)
# link_directories(${gSOAP_PATH}/lib)
# set(LIBS ${gSOAP_LIB})

# 1、2章节的服务器端为CGI的章节，不建议学习，看看就好，无需执行，浪费时间
# 建议从章节3开始学习, 服务器端为单独程序，从此学习
set(CHAPTERS1
    1-hello_world
    2-hello_world-rest-api
    3-1-calc
    3-3-calc
)

# 执行CHAPTERS1中的所有代码
foreach(CHAPTER ${CHAPTERS1})

    file(GLOB HAS_NSMAP "${CMAKE_CURRENT_BINARY_DIR}/${CHAPTER}/*.nsmap")
    if("${HAS_NSMAP}" STREQUAL "")    # 判断HAS_NSMAP是否为空
        file(GLOB HEADS "${CHAPTER}/*.h")
        message("${CHAPTER}")
        message("${CMAKE_CURRENT_BINARY_DIR}/${CHAPTER}")
        #使用soapcpp2.exe -d path 指定生成路径
        make_directory(${CMAKE_CURRENT_BINARY_DIR}/${CHAPTER}) #先创建文件夹，不然会执行soapcpp2会失败
        execute_process(COMMAND ${gSOAP_BIN} -d ${CMAKE_CURRENT_BINARY_DIR}/${CHAPTER} ${HEADS})
    else()
        message("已经生成了：" ${HAS_NSMAP})
    endif()

    # 输出执行文件路径
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/${CHAPTER})
    add_subdirectory(${CHAPTER})
    
endforeach(CHAPTER)

# 服务器端为单独程序，从此学习
set(CHAPTERS2
    3-2-calc
    3-4-calc
    3-5-calc
    3-6-calc
)

# 执行CHAPTERS2中的所有代码
foreach(CHAPTER ${CHAPTERS2})

    file(GLOB HAS_NSMAP "${CMAKE_CURRENT_BINARY_DIR}/${CHAPTER}/*.nsmap")
    if("${HAS_NSMAP}" STREQUAL "")    # 判断HAS_NSMAP是否为空
        file(GLOB HEADS "${CHAPTER}/*.h")
        message("${CHAPTER}")
        message("${CMAKE_CURRENT_BINARY_DIR}/${CHAPTER}")
        #使用soapcpp2.exe -d path 指定生成路径, -i 生成C++服务代理和继承自soap结构的对象
        make_directory(${CMAKE_CURRENT_BINARY_DIR}/${CHAPTER}) #先创建文件夹，不然会执行soapcpp2会失败
        execute_process(COMMAND ${gSOAP_BIN} -d ${CMAKE_CURRENT_BINARY_DIR}/${CHAPTER} -i ${HEADS})
        message("head = " ${HEADS})
    else()
        message("已经生成了：" ${HAS_NSMAP})
    endif()
    
    # 输出执行文件路径
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/${CHAPTER})
    add_subdirectory(${CHAPTER})
    
endforeach(CHAPTER)

