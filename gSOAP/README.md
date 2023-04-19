# gsoap
```
gsoap官方Developer Center：https://www.genivia.com/dev.html
gsoap源码下载地址：https://sourceforge.net/projects/gsoap2/files/
gsoap使用总结：https://www.cnblogs.com/liushui-sky/p/9723397.html
gsoap User guide: https://www.genivia.com/doc/guide/html/index.html
```

# 编译
## 设置依赖文件夹thirdparty
```
mkdir thirdparty
本人的tree thirdparty如下：
gsoap-2.8
    ├─bin           --必须, 在gsoap源码中的 gsoap-2.8\gsoap\bin\win64 中拷贝
    ├─include       --必须, stdsoap2.h 在gsoap源码中拷贝
    ├─lib
    │  └─pkgconfig
    ├─plugin        --必须, 在gsoap源码中拷贝
    ├─share         
    │  └─gsoap
    │      ├─custom
    │      ├─extras
    │      ├─import
    │      ├─plugin
    │      └─WS
    └─src           --必须, stdsoap2.cpp 在gsoap源码中拷贝
```
## windows+cygwin编译
```
mkdir build 
cd build
cmake -G "Unix Makefiles" ..
make -j 4
```
## windows+mingw编译
```
mkdir build 
cd build
cmake -G "MinGW Makefiles" .. 
make -j 4
注意: 3-6-calc 的暂未适配mingw
```
## linux下编译
### 不编译gsoap：
```
可把windows下执行过 cmake -G "Unix Makefiles" .. 后的整个项目拷贝至linux。
处理build目录，保留生成的1-hello_world至3-6-calc文件夹，把其他删除，重新执行cmake ..
```
### 编译gsoap：
```
把执行文件拷贝至 thirdparty/gsoap-2.8/bin
mkdir build 
cd build
cmake -G "Unix Makefiles" ..
make -j 4
```

# 章节1和2的服务端为CGI程序，不建议学习，建议从章节3开始学习。
## 如需配置CGI，请看：Windows 配置Apache+CGI ：https://www.cnblogs.com/music-liang/p/11846268.html
### 配置端口为8080
```
windows cmd下查看8080端口是否被占用
tasklist|findstr "8080"
```
### 参照子目录下CMakeLists.txt的改名提示把.exe改名为.cgi放到指定的cgi-bin目录下

# 章节3开始为单独的服务端（stand-alone service）
```
3-1-calc        --      C风格的calc服务端）, 客户端
3-2-calc        --      C++风格的calc服务端, 客户端
3-3-calc        --      C风格的calc多线程服务端（multi-threaded stand-alone service）, 客户端
3-4-calc        --      C++风格的calc多线程服务端, 客户端
3-5-calc        --      C++风格的calc多线程服务端: 实现一个简单的计算加法post的html, 通过浏览器访问测试
3-6-calc        --      C++风格的calc多线程服务端: 使用select socket, 实现一个简单的计算加法post的html, 通过浏览器访问测试
```