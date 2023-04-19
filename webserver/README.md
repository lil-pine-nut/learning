# 这是 libuv + http-parser 的测试
```
从 https://github.com/springmeyer/libuv-webserver 中获取webserver的代码。

修改了当文件过大（大于65536字节）时会导致程序崩溃的问题。

添加了一些其他文件类型的Content-Type。

添加了URL的编码和解码，在拼接html字符串的Content-Type 后添加 "; charset=utf-8"，防止浏览器显示中文乱码。

#define DEBUG 可debug信息

添加了Markdown转HTML：https://github.com/HiSunzhenliang/MarkdownParser ，改读Markdown文件为std::istringstream(std::string)；修复了一个bug；添加了转为HTML时显示尖括号的问题。

可用浏览器访问IP端口测试, 把文件图片等放置执行程序的同目录或子目录下可访问。注意：修改libuv-webserver.cpp里github-markdown.css的IP为自己的服务器IP。
```

# libuv-webserver-openssl.cpp 添加了openssl作为的https握手加密解密信息
## 主要参考：https://github.com/darrenjs/openssl_examples ，其使用了socket+openssl的握手和加解密, socket和 openssl的使用如下：
```
  +------+                                    +-----+
  |......|--> read(fd) --> BIO_write(rbio) -->|.....|--> SSL_read(ssl)  --> IN
  |......|                                    |.....|
  |.sock.|                                    |.SSL.|
  |......|                                    |.....|
  |......|<-- write(fd) <-- BIO_read(wbio) <--|.....|<-- SSL_write(ssl) <-- OUT
  +------+                                    +-----+

          |                                  |       |                     |
          |<-------------------------------->|       |<------------------->|
          |         encrypted bytes          |       |  unencrypted bytes  |
```

## 次要参考：https://github.com/DenisLug/OpenSSL-Client-Server-with-BIO ，其使用了socket+openssl的实现tcp


# libuv-webserver.cpp添加了mp4在线播放
## Chrome内核（测试谷歌浏览器和Microsoft Edge支持）支持直接播放，非Chrome内核：IE浏览器11用html的ckplayer-x2(拉动进度条会有问题), 而Video.js(在IE11只会请求一次...)，原因大概是http传输MP4不够完美？故而只提供下载。