# 这是 hbase thrift 的测试使用

## 说明：带2的文件目录为使用thrift2接口
```
demo - hbase thrift 的 简单接口
Hbase-Client-Class - 修改于：hbase-thrift-master，修改为支持 thrift-0.10.0, 参考：https://github.com/ypf412/hbase-thrift
hbase-thrift-gen-cpp - 基于hbase的thrift生成的接口文件
```

# 编译boost
```
注意：编译thrift需要用到boost::math的库，最新版似乎移除了该库。测试编译thrift使用boost_1_64_0不会出错。
boost源码各版本下载：https://sourceforge.net/projects/boost/files/boost/
tar zxvf boost_1_64_0.tar.gz
cd boost_1_64_0/
./bootstrap.sh
#编译静态库，设置安装路径
./b2 cflags='-fPIC' cxxflags='-fPIC' install --prefix=/home/lws/software/boost_1_64_0 
```
## 设置环境变量
```
vim ~/.bashrc 填入如下：
# boost_1_64_0
export CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:/home/lws/software/boost_1_64_0/include/
export C_INCLUDE_PATH=$C_INCLUDE_PATH:/home/lws/software/boost_1_64_0/include/
export LIBRARY_PATH=$LIBRARY_PATH:/home/lws/software/boost_1_64_0/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/lws/software/boost_1_64_0/lib

source ~/.bashrc 使环境变量生效
```
# 编译thrift（thrift-0.10.0）
```
wget http://archive.apache.org/dist/thrift/0.10.0/thrift-0.10.0.tar.gz
tar zxvf thrift-0.10.0.tar.gz
cd thrift-0.10.0/

./configure CFLAGS='-fPIC' CXXFLAGS='-fPIC'  --with-boost=/home/lws/software/boost_1_64_0 --prefix=/home/lws/software/thrift-0.10.0 
make -j 16
make install
```
# 编译thrift（thrift-0.16.0）
```
参考编译thrift-0.10.0
thrift-0.10.0生成的接口默认使用boost的智能指针；
thrift-0.13.0及以上版本是生成的接口默认使用c++11的智能指针。
wget http://archive.apache.org/dist/thrift/0.16.0/thrift-0.16.0.tar.gz 
tar zxvf thrift-0.16.0.tar.gz
cd thrift-0.16.0/
#设置安装路径，设置boost库路径
注意：thrift-0.13.0及以上：
./configure CFLAGS='-fPIC' CXXFLAGS='-fPIC'  --with-boost=/home/lws/software/boost_1_64_0 --prefix=/home/lws/software/thrift-0.16.0
make -j 16
make install
```
# 生成接口文件（thrift或thrift2）
```
进入thrift安装目录：cd /home/lws/softwares/thrift-0.10.0/bin/
hbase源码下载：http://archive.apache.org/dist/hbase/
或者直接 wget http://archive.apache.org/dist/hbase/1.3.0/hbase-1.3.0-src.tar.gz
（注意：测试hbase-2.4.2生成thrift2/hbase.thrift的cpp代码编译时存在未定义）
tar zxvf hbase-1.3.0-src.tar.gz
生成thrift：
./thrift --gen cpp hbase-1.3.0/hbase-thrift/src/main/resources/org/apache/hadoop/hbase/thrift/Hbase.thrift
将生成的gen-cpp目录拷贝到工程文件夹下

或者生成thrift2：（不包含ddl 操作：创建表、修改表、删除表等）
./thrift --gen cpp hbase-1.3.0/hbase-thrift/src/main/resources/org/apache/hadoop/hbase/thrift2/hbase.thrift
将生成的gen-cpp目录拷贝到工程文件夹下
```