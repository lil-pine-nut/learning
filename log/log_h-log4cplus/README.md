# 这是每天定时产生文件日志+控制台同时输出

## 资源
log4cplus：https://github.com/log4cplus/log4cplus
log4cplus配置文件编写及使用: https://blog.csdn.net/fksec/article/details/41546189
详细的属性配置请看：log4cplus.properties, 这是到分钟的，到日，需要修改Log.cpp和log4cplus.properties的FilenamePattern的参数

v2.0以上更改为c++11实现。
v1支持c++98，本人下载时最新版为v1.2.2。

## 存在问题
### RollOnClose不为false时，可能会报段错误，修改方式：
https://github.com/log4cplus/log4cplus/issues/272


### v1.2.2 使用TimeBasedRollingFileAppender：
当开启 log4cplus.appender.LogFile.RollOnClose=true，会设置为我们设置为false，不使用这个功能。
或者修改源码：参照RollOnClose不为false时，可能会报段错误的修改方式。

### v1.2.2 DailyRollingFileAppender 会创建一个空文件
执行完程序后会创建一个新的文件，程序结束前会在rollover函数进行文件Rolling（删除MaxBackupIndex的文件，一步步更改文件名）, 并创建新文件。
修改：
```
在DailyRollingFileAppender::close()函数的前面第一行添加：
closed = true;

接着在 DailyRollingFileAppender::rollover(bool alreadyLocked)
找到下面if{}里的语句修改为：
if(!closed)
{
    // Open a new file, e.g. "log".
    open(std::ios::out | std::ios::trunc);
    loglog_opening_result (loglog, out, filename);
}
```

## 使用

```
linux下： ./set.sh
```

# DailyRollingFileAppender 在 log4cplus 和 log4cxx
log4cplus-2.properties 为 log4cplus的DailyRollingFileAppender的属性配置
```
log4cplus 当RollOnClose不为false, 每次执行一个程序，在记录期内, 生成这样：log.log.2022-04-06 log.log.2022-04-06.1 log.log 2022-04-06.2

log4cxx-0.9.7 的DailyRollingFileAppender每次执行一个程序，在记录期内, 生成这样：log.log。在下一个记录期执行程序，上一个记录期的文件保存为 log.log.2022-04-06，新的记录期文件为: log.log
log4cxx-0.9.7 如果程序执行完还没到下一记录期，则下次执行不会创建新文件，会直到下一天才创建新文件把昨日的文件加上DataPattern（注意：log4cxx-0.11.0为DatePattern，真是问题多多...，建议别用log4cxx）的后缀

log4cxx-0.11.0 则跟log4cplus-v1.2.2一样不会在第二天生成新文件

通过  touch -d "24 hours ago" fileneame 可以修改文件的修改日期为24小时前进行测试

```

# log4cplus的DailyRollingFileAppender 的Rolling
```
构造时init获取下一个每次开始获取nextRolloverTime，当close()和doAppend()会进行判断进行文件滚动.
尝试修改为上面说的log4cxx的DailyRollingFileAppender的样式，但是Linux似乎无代码获取文件的创建时间，就不改了。stat不行：https://zhuanlan.zhihu.com/p/150235061

稍微看了log4cxx的源码，当log4cplus使用DailyRollingFileAppender, 且设置 RollOnClose=false, 可以在DailyRollingFileAppender::init 添加判断文件上次的修改时间为昨日则执行一次rollover(); 非连续执行程序便可像log4cxx那样到第二天才滚动生成一个新文件。
```