#include <iostream>
#include <curl/curl.h>
#include <string.h>
#include <stdio.h>

using namespace std;
//回调函数  得到响应内容
int write_data(void *buffer, int size, int nmemb, void *userp)
{
    std::string *str = dynamic_cast<std::string *>((std::string *)userp);
    str->append((char *)buffer, size * nmemb);
    return nmemb;
}
bool upload(string url, string *response);

int main(int argc, char **argv)
{

    std::string body;
    std::string response;

    int status_code = upload("http://openapi.heclouds.com/application?action=CreateDeviceFile&version=1", &response);
    if (!status_code)
    {
        return -1;
    }
    cout << response << endl;
    if (response.empty())
    {
        cerr << "post response message is empty !!!" << endl;
        return 0;
    }
    string error_msg;
    string uuid;
    size_t pos1 = response.find("\"success\":");
    size_t pos2 = response.find("}", pos1 + 10);
    if (pos1 == string::npos || pos2 == string::npos)
    {
        cerr << "post response cannot find the sign of \"success\" !!!" << endl;
        return 0;
    }
    string post_flag = response.substr(pos1 + 10, pos2 - (pos1 + 10));
    if (post_flag != "true")
    {
        pos1 = response.find("\"msg\":\"");
        pos2 = response.find("\",", pos1 + 7);
        if (pos1 != string::npos || pos2 != string::npos)
        {
            error_msg = response.substr(pos1 + 7, pos2 - (pos1 + 7));
        }
        cerr << "error_msg:" << error_msg << endl;
        return 0;
    }
    else
    {
        cerr << "post_flag == \"true\"" << endl;
        pos1 = response.find("\"fid\":\"");
        pos2 = response.find("\"},", pos1 + 7);
        if (pos1 != string::npos || pos2 != string::npos)
        {
            uuid = response.substr(pos1 + 7, pos2 - (pos1 + 7));
        }
        cerr << "uuid:" << uuid << endl;
    }

    return 0;
}

bool upload(string url, string *response)
{

    CURL *curl;
    CURLcode ret;
    curl = curl_easy_init();

    if (curl)
    {

        // 读取上传文件
        FILE *file = fopen(Curl_Version_TXT, "rb");
        if (file == NULL)
        {
            cerr << "fopen " << Curl_Version_TXT << " Failed" << endl;
            return false;
        }
        fseek(file, 0, SEEK_END);
        unsigned size = ftell(file);
        fseek(file, 0, SEEK_SET);
        string file_str;
        file_str.resize(size);
        int fread_size = fread(&file_str[0], size, 1, file);
        cerr << "fread:" << fread_size << endl;
        cerr << "file_str.size():" << file_str.size() << endl;

        // 设置curl的请求头
        struct curl_slist *header_list = NULL;
        header_list = curl_slist_append(header_list, "Authorization: version=2020-05-29&res=userid%2F289792&et=2547098723&method=md5&sign=%2BhiXVbH972GoKrXQ3N1S8g%3D%3D");
        // header_list = curl_slist_append(header_list, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
        curl_easy_setopt(curl, CURLOPT_URL, (char *)url.c_str()); //指定url

        // 构造post参数 - body
        struct curl_httppost *post = NULL;
        struct curl_httppost *last = NULL;
        curl_formadd(&post, &last, CURLFORM_PTRNAME, "device_name", CURLFORM_PTRCONTENTS, "test_subdev_01", CURLFORM_END);
        curl_formadd(&post, &last, CURLFORM_PTRNAME, "product_id", CURLFORM_PTRCONTENTS, "Y8CLDa6OfI", CURLFORM_END);
        curl_formadd(&post, &last, CURLFORM_PTRNAME, "md5", CURLFORM_PTRCONTENTS, "16d9080f1223b1c0a3cb41e1355ba91a", CURLFORM_END);
        curl_formadd(&post, &last, CURLFORM_PTRNAME, "size", CURLFORM_PTRCONTENTS, "116", CURLFORM_END);
        curl_formadd(&post, &last, CURLFORM_PTRNAME, "filename", CURLFORM_PTRCONTENTS, "test-curl_version.txt", CURLFORM_END);
        // curl_formadd(&post, &last, CURLFORM_PTRNAME,  "file", CURLFORM_FILE, "./test.jpg",CURLFORM_FILENAME, "hello.jpg", CURLFORM_END);// form-data key(file) "./test.jpg"为文件路径  "hello.jpg" 为文件上传时文件名

        // 直接输入上传文件
        // curl_formadd(&post, &last, CURLFORM_PTRNAME, "file", CURLFORM_FILE, "/data4/lws/demo/test-curl_version.txt", CURLFORM_END);

        // 读上传文件至buff后上传
        curl_formadd(&post, &last,
                     CURLFORM_PTRNAME, "file",
                     CURLFORM_BUFFER, "test-curl_version.txt",
                     CURLFORM_BUFFERPTR, file_str.c_str(),
                     CURLFORM_BUFFERLENGTH, file_str.size(),
                     CURLFORM_END);

        // 计算文件md5 md5sum /data4/lws/demo/test-curl_version.txt
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);

        //不接收响应头数据0代表不接收 1代表接收, 默认不接收
        // curl_easy_setopt(curl, CURLOPT_HEADER, 0);

        /* 打开完整的协议/调试输出*/
        // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);   //绑定相应
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)response); //绑定响应内容的地址

        ret = curl_easy_perform(curl); //执行请求

        // 清空数据
        curl_slist_free_all(header_list);
        curl_formfree(post);

        if (ret != CURLE_OK)
        {
            cerr << "error: " << curl_easy_strerror(ret) << endl;
            curl_easy_cleanup(curl);
            return false;
        }
        curl_easy_cleanup(curl);
        return true;
    }
    return false;
}
