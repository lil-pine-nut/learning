# 这是每天定时产生文件日志+控制台同时输出

## 资源
spdlog：https://github.com/gabime/spdlog
spdlog wiki：https://github.com/gabime/spdlog/wiki
set_pattern 参考： https://github.com/gabime/spdlog/wiki/3.-Custom-formatting
别人的学习笔记：https://www.cnblogs.com/oucsheep/p/8426548.html

## 编译

```
linux下： ./set.sh

通过修改cmake xxx .. 的参数选择编译

如下：

使用find_package查找spdlog:     -DUSE_FIND_PACKAGE=ON
自己指定spdlog:                 -DUSE_FIND_PACKAGE=OFF

Only Head 编译:                 -DSPDLOG_BUILD_EXAMPLE_HO=ON
lib link 编译:                  -DSPDLOG_BUILD_EXAMPLE_HO=OFF
```