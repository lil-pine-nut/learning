#include <stdio.h>
#include <iostream>
#include <curl/curl.h>

struct data
{
    FILE *our_fd;
};

static int seek_cb(void *userp, curl_off_t offset, int origin)
{
    struct data *d = (struct data *)userp;
    fseek(d->our_fd, offset, origin);
    std::cout << "fseek = " << offset << ", origin = " << origin << std::endl;
    return CURL_SEEKFUNC_OK;
}

struct FtpFile
{
    const char *filename;
    FILE *stream;
};

static size_t my_fwrite(void *buffer, size_t size, size_t nmemb, void *stream)
{
    struct FtpFile *out = (struct FtpFile *)stream;
    fseek((FILE *)out->stream, size, nmemb);
    if (out && !out->stream)
    {
        /* 打开文件以进行写操作 */

        if (!out->stream)
            return -1; /* failure, can't open file to write */
    }
    return fwrite(buffer, size, nmemb, out->stream);
}

int main(void)
{
    CURL *curl;
    CURLcode res;
    struct FtpFile ftpfile = {
        "curl.txt", /* 若FTP下载成功，名命下载后的文件为"curl.txt" */
        NULL};
    ftpfile.stream = fopen(ftpfile.filename, "wb");
    const char *urlkey = "ftpuser:123456"; //服务器用户名密码
    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL,
                         "ftp://127.0.0.1:2121/a.txt"); //下载指定的文件

        /* 设置ftp账号密码 */
        curl_easy_setopt(curl, CURLOPT_USERPWD, urlkey);

        /* 定义回调函数，以便在需要写入数据时进行调用 */
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_fwrite);

        struct data seek_data;
        seek_data.our_fd = ftpfile.stream;
        curl_easy_setopt(curl, CURLOPT_SEEKFUNCTION, seek_cb);
        curl_easy_setopt(curl, CURLOPT_SEEKDATA, &seek_data);

        /*设置一个指向我们的结构的指针传递给回调函数*/
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ftpfile);
        curl_easy_setopt(curl, CURLOPT_RANGE, "0-100"); ///设置前面部分 100-999
        /* 打开完整的协议/调试输出*/
        // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        res = curl_easy_perform(curl);

        /* 释放所有curl资源*/
        curl_easy_cleanup(curl);

        if (CURLE_OK != res)
        {
            /*容错处理 */
            fprintf(stderr, "curl told us %d\n", res);
        }
        else
        {
            /* now extract transfer info */
            curl_off_t speed_upload, total_time;
            curl_easy_getinfo(curl, CURLINFO_SPEED_UPLOAD_T, &speed_upload);
            curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME_T, &total_time);

            fprintf(stderr, "Speed: %" CURL_FORMAT_CURL_OFF_T " bytes/sec during %" CURL_FORMAT_CURL_OFF_T ".%06ld seconds\n",
                    speed_upload,
                    (total_time / 1000000), (long)(total_time % 1000000));
        }
    }

    if (ftpfile.stream)
        fclose(ftpfile.stream); /* 关闭本地文件 */

    /*释放所有curl资源*/
    curl_global_cleanup();
    printf("End of ftptestdownload\n\n");
    return 0;
}