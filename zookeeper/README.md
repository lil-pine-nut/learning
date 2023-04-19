# 这是 zookeeper C API的使用测试

```
test_init 测试 zookeeper_init
get_value 测试 获取zookeeper的节点值
QueryServer 使用 QueryServerd 检测 QueryServer创建的zookeeper临时节点判断程序是否在在线，不在线则重启。
Configuration_Management 测试 zookeeper 管理配置（数据库的账号密码那些等等）

```

# 编译

```
1、获取mvn
wget https://mirrors.cnnic.cn/apache/maven/maven-3/3.8.5/binaries/apache-maven-3.8.5-bin.tar.gz
解压，把apache-maven-3.8.5/bin添加到环境变量

2、获取源码（apache-zookeeper-3.6.4）
wget --no-check-certificate https://archive.apache.org/dist/zookeeper/zookeeper-3.6.4/apache-zookeeper-3.6.4.tar.gz
解压进入目录

3、编译
在apache-zookeeper-3.6.4目录下，执行：
mvn compile
cd apache-zookeeper-3.6.4/zookeeper-client/zookeeper-client-c
mkdir build && cd build
cmake -DWANT_CPPUNIT=OFF .. 
或者指定openssl (编译时./config -fPIC，先移动openssl的静态库在其他位置)：cmake -DWANT_CPPUNIT=OFF -DOPENSSL_ROOT_DIR=/home/lws/software/openssl-1.1.1q ..
make
建议：生成动态库，生成的静态库无法打包指定的openssl静态库，动态库则可以，把zookeeper-client/zookeeper-client-c/CMakeLists.txt的add_library()中的STATIC改为SHARED（有两个）。接着重新cmake .. 后make。

4、所需头文件：include目录、generated/zookeeper.jute.h
```