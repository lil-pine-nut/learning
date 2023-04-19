#include "curl-client.h"
#include <iostream>

using namespace std;

int main()
{
    /* 此次在127.0.0.1 的 lws 用户下测试 */

    CurlClient client;
    std::cout << "client.connect = " << client.connect("lws@127.0.0.1", "lws", "luoweisong", "/home/lws/.ssh/id_rsa.pub", CurlClient::SFTP, true) << std::endl;

    // std::cout<<"put:"<<std::endl;
    // client.put("/home/lws/learn-curl/test-file-folder/curl-7.45.0.tar.gz", "/home/lws/test-ftp-recv/curl-7.45.0.22.tar.gz");

    // std::cout<<"get:"<<std::endl;
    // client.get("/home/lws/test-ftp-recv/curl-7.45.0.22.tar.gz", "./test-curl-7.45.0.22.tar.gz");

    // 查看目录要往文件夹路径后加/
    std::cout << "ls:" << std::endl;
    std::string listinfo;
    client.ls("/data1/MR_MDT_csv/ZTE/MDT/", listinfo);
    std::cout << "listinfo = " << listinfo << std::endl;

    // std::cout<<"mv:"<<std::endl;
    // client.mv("/home/lws/test-ftp-recv/curl-7.45.0.22.tar.gz", "/home/lws/test-ftp-recv/curl-7.45.0.22-mv.tar.gz");

    // std::cout<<"mkdir:"<<std::endl;
    // client.mkdir("/home/lws/test101");
    // std::cout<<"test.cpp end"<<std::endl;
}