#include <stdio.h>
#include <string.h>
#include <iostream>
#include <curl/curl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#define LOCAL_FILE "/home/lws/learn-curl/test-file-folder/new-url-rule.txt" //要上传的文件
#define UPLOAD_FILE_AS "a.txt"
#define REMOTE_URL "ftp://127.0.0.1:2121/test/" UPLOAD_FILE_AS // FTP服务器地址
#define RENAME_FILE_TO "b.txt"

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

static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *stream)
{
    curl_off_t nread;
    fseek((FILE *)stream, size, nmemb);
    size_t retcode = fread(ptr, size, nmemb, (FILE *)stream);

    nread = (curl_off_t)retcode;

    // fprintf(stderr, " We read %" CURL_FORMAT_CURL_OFF_T
    //         " bytes from file\n", nread);
    return nread;
}

int main(void)
{
    CURL *curl;
    CURLcode res;
    FILE *hd_src;
    struct stat file_info;
    curl_off_t fsize;

    const char *urlkey = "ftpuser:123456"; //服务器用户名密码
    struct curl_slist *headerlist = NULL;
    static const char buf_1[] = "RNFR " UPLOAD_FILE_AS;
    static const char buf_2[] = "RNTO " RENAME_FILE_TO;

    /* 获得上传文件的大小 */
    if (stat(LOCAL_FILE, &file_info))
    {
        printf("Couldn't open '%s': %s\n", LOCAL_FILE, strerror(errno));
        return 1;
    }
    fsize = (curl_off_t)file_info.st_size;

    printf("Local file size: %" CURL_FORMAT_CURL_OFF_T " bytes.\n", fsize);

    /* 获得FILE类型变量 */
    hd_src = fopen(LOCAL_FILE, "rb");

    /* 初始化 */
    curl_global_init(CURL_GLOBAL_ALL);

    /* 获得curl操作符 */
    curl = curl_easy_init();
    if (curl)
    {
        /*建立一个传递给libcurl的命令列表 */
        headerlist = curl_slist_append(headerlist, buf_1);
        headerlist = curl_slist_append(headerlist, buf_2);

        struct data seek_data;
        seek_data.our_fd = hd_src;
        curl_easy_setopt(curl, CURLOPT_SEEKFUNCTION, seek_cb);
        curl_easy_setopt(curl, CURLOPT_SEEKDATA, &seek_data);
        curl_easy_setopt(curl, CURLOPT_MAX_RECV_SPEED_LARGE, (curl_off_t)65526 * 10);
        curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, (curl_off_t)65536 * 10); //最大是65536字节,无法设置了

        /* 设置ftp账号密码 */
        curl_easy_setopt(curl, CURLOPT_USERPWD, urlkey);

        /* 使用curl提供的Read功能 */
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);

        /* 上传使能 */
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

        /* 设置特定目标 */
        curl_easy_setopt(curl, CURLOPT_URL, REMOTE_URL);

        /* 传递最后一个FTP命令以在传输后运行 */
        curl_easy_setopt(curl, CURLOPT_POSTQUOTE, headerlist);

        /*指定上传文件 */
        curl_easy_setopt(curl, CURLOPT_READDATA, hd_src);

        /*设置要上传的文件的大小（可选） */
        // curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE,
        //                 (curl_off_t)fsize);
        curl_easy_setopt(curl, CURLOPT_RANGE, "100-999"); ///设置前面部分 100-999
        /* 打开完整的协议/调试输出*/
        // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        //创建目录
        curl_easy_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS, CURLFTP_CREATE_DIR);

        curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, 120000L);

        /* 运行 */
        res = curl_easy_perform(curl);
        /* 容错处理 */
        if (res != CURLE_OK)
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
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

        /* 清除FTP命令列表 */
        curl_slist_free_all(headerlist);

        /*释放所有curl资源 */
        curl_easy_cleanup(curl);
    }
    fclose(hd_src); /*关闭本地文件 */

    /*释放所有curl资源 */
    curl_global_cleanup();

    printf("End of ftptestupload\n\n");
    return 0;
}