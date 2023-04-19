# 这是每天定时产生文件日志+控制台同时输出

## 资源
log4cxx：https://github.com/apache/logging-log4cxx
log4cxx主页： http://logging.apache.org/log4cxx
log4cxx配置文件（properties文件）详解: https://wenku.baidu.com/view/7fc2b72301020740be1e650e52ea551811a6c952.html
Log4j.properties配置详解: https://www.jianshu.com/p/ccafda45bcea

0.12.0及以上为第一个需要C++11的版本。所以下载 0.11.0

## 编译log4cxx
### 编译apr
wget http://archive.apache.org/dist/apr/apr-1.6.5.tar.gz
解压进入
./configure --prefix="your apr path"
export CFLAGS=-fPIC
make -j 4 && make install
### apr-util
wget http://archive.apache.org/dist/apr/apr-util-1.6.1.tar.gz
./configure --prefix="your apr-util path" --with-apr="your apr path"
export CFLAGS=-fPIC
make -j 4 && make install

### 编译log4cxx源码
建议从GitHub上下载log4cxx-0.11.0的版本解压进入
#### ./autogen.sh成功
./configure --prefix="your log4cxx path" --with-charset=utf-8 --with-apr="your apr path" --with-apr-util="your apr-util path"
export CFLAGS=-fPIC
make -j 4 && make install

#### ./autogen.sh失败则用cmake
修改 src\cmake\FindAPR.cmake
    在 find_package_handle_standard_args 前一行添加：
    set(APR_INCLUDE_DIR /your apr path/include/apr-1)
    set(APR_LIBRARIES /your apr path/lib/libapr-1.a)

修改 src\cmake\FindAPR-Util.cmake
    在 find_package_handle_standard_args 前一行添加：
    set(APR_UTIL_INCLUDE_DIR /your apr-util path/include/apr-1)
    set(APR_UTIL_LIBRARIES /your apr-util path/lib/libaprutil-1.a)

回到logging-log4cxx-0.11.0目录
mkdir build && cd build
cmake -DBUILD_TESTING=off -DCMAKE_INSTALL_PREFIX="your log4cxx path" -DAPU_STATIC=yes -DAPR_STATIC=yes -DBUILD_SHARED_LIBS=ON -DLOG4CXX_CHARSET=""utf-8 ..
export CFLAGS=-fPIC
make -j 4 && make install

### 编译log4cxx的0.9.7源码不需要apr和apr-util
从GitHub中获取源码，解压进入
./autogen.sh
./configure  --prefix="/home/lws/software/logging-log4cxx-0_9_7" --with-charset=utf-8
make    #使用make -j 4 会出现无法预料的报错
make install
#### 根据报错修改源码
1、vim include/log4cxx/xml/domconfigurator.h
把 String DOMConfigurator::subst(const String& value);
修改为 ：String subst(const String& value);
include/log4cxx/xml/domconfigurator.h
2、往报错的源文件添加头文件： #include <string.h>

## 使用
根据set.h, 选择 -DUSE_NEW_LOG4CXX=ON 或者 -DUSE_NEW_LOG4CXX=OFF 使用 log4cxx-0.11.0 或 log4cxx-0.9.7

# log4cxx-0.9.7 和 log4cxx-0.11.0的不同
1、log4cxx-0.9.7编译不需要apr和apr-util
2、log4cxx-0.9.7没有trace的日志等级，头文件中forcedLog的为：
void forcedLog(const LevelPtr& level, const String& message, const char* file=0, int line=-1); 其中level的参数为: log4cxx::Level::DEBUG, log4cxx::Level::INFO ...
log4cxx-0.11.0有trace的日志等级，头文件中forcedLog的为：
void forcedLog(const LevelPtr& level, const std::string& message, const log4cxx::spi::LocationInfo& location); 其中level的参数为: log4cxx::Level::getTrace(), log4cxx::Level::getDebug() ...
3、必须注意：log4cxx-0.9.7的 DailyRollingFileAppender 为 DataPattern ；log4cxx-0.11.0的 DailyRollingFileAppender 为 DatePattern
4、log4cxx-0.9.7不会主动创建文件夹存放log，log4cxx-0.11.0会。

# DailyRollingFileAppender 在 log4cplus 和 log4cxx
```
log4cplus 当RollOnClose不为false, 每次执行一个程序，在记录期内, 生成这样：log.log.2022-04-06 log.log.2022-04-06.1 log.log 2022-04-06.2

log4cxx-0.9.7 的DailyRollingFileAppender每次执行一个程序，在记录期内, 生成这样：log.log。在下一个记录期执行程序，上一个记录期的文件保存为 log.log.2022-04-06，新的记录期文件为: log.log
log4cxx-0.9.7 如果程序执行完还没到下一记录期，则下次执行不会创建新文件，会直到下一天才创建新文件把昨日的文件加上DataPattern（注意：log4cxx-0.11.0为DatePattern，真是问题多多...，建议别用log4cxx）的后缀

log4cxx-0.11.0 则跟log4cplus-v1.2.2一样不会在第二天生成新文件

通过  touch -d "24 hours ago" fileneame 可以修改文件的修改日期为24小时前进行测试

```