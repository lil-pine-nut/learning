#include <iostream>
#include <curl/curl.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <set>
#include <map>
#include <string.h>

using namespace std;

typedef map<string, CURL *> ListCurlMap;
ListCurlMap m_ListCurlMap;

int GetFtpFileList(void *ptr, size_t size, size_t nmemb, void *stream)
{
    std::string &buffer = *(static_cast<std::string *>(stream));
    char *pBuf = (char *)ptr;
    size_t len = size * nmemb;
    buffer += string(pBuf, len);
    return len;
}

size_t WriteFileCallback(void *ptr, size_t size, size_t nmemb, void *stream)
{
    std::ofstream *pFileStream = reinterpret_cast<std::ofstream *>(stream);
    if (pFileStream->is_open())
    {
        pFileStream->write(reinterpret_cast<char *>(ptr), size * nmemb);
        return size * nmemb;
    }
    return -1;
}

#include <math.h>

#include <sys/time.h>
#include <unistd.h>
typedef unsigned long long guint64;
guint64 GetCurrentMicroTime()
{
    struct timeval dwStart;
    gettimeofday(&dwStart, NULL);
    guint64 dwTime = 1000000 * dwStart.tv_sec + dwStart.tv_usec;
    return dwTime;
}

void ls(string remote, vector<string> *filename_vec, vector<string> *all_filename_vec)
{
    string info;
    CURL *curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, remote.c_str());

    // 允许与服务器的连接阶段所花费的最长时间
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 1L);

    // 允许服务器在会话被认为是死的之前为一个命令发送一个响应信息的时间
    curl_easy_setopt(curl, CURLOPT_FTP_RESPONSE_TIMEOUT, 1L);

    // curl_easy_setopt(curl, CURLOPT_DIRLISTONLY, 1L);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "LIST");

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, GetFtpFileList);
    /* Set a pointer to our struct to pass to the callback */
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &info);

    /* 打开完整的协议/调试输出*/
    // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
        std::cerr << "curl_easy_perform(" << res << ") failed: " << curl_easy_strerror(res) << std::endl;
    curl_easy_cleanup(curl);

    if (info.empty())
    {
        cerr << "info.empty()" << endl;
        return;
    }

    cerr << "info = " << info << endl;
}

void get(const char *remote_file_path, const char *local_file_path)
{
    guint64 now_time = GetCurrentMicroTime();
    std::ofstream filestream;
    filestream.open(local_file_path, std::ifstream::out | std::ifstream::binary);
    if (!filestream)
    {
        cerr << "open local_file_path filed!" << endl;
        return;
    }

    CURL *curl = curl_easy_init();
    if (curl)
    {
        /* specify target */
        curl_easy_setopt(curl, CURLOPT_URL, remote_file_path);

        // 允许与服务器的连接阶段所花费的最长时间
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 1L);

        // 允许服务器在会话被认为是死的之前为一个命令发送一个响应信息的时间
        curl_easy_setopt(curl, CURLOPT_FTP_RESPONSE_TIMEOUT, 1L);

        /* 定义回调函数，以便在需要写入数据时进行调用 */
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteFileCallback);

        /*设置一个指向我们的结构的指针传递给回调函数*/
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &filestream);

        /* 打开完整的协议/调试输出*/
        // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        /* 运行 */
        CURLcode res = curl_easy_perform(curl);

        /* 容错处理 */
        if (res != CURLE_OK)
        {
            std::cerr << "curl_easy_perform(" << res << ") failed: " << curl_easy_strerror(res) << std::endl;
        }

        /* always cleanup */
        curl_easy_cleanup(curl);
    }

    filestream.close();

    cerr << "Get File Cost: " << GetCurrentMicroTime() - now_time << " us." << endl;
}

void ls_Init_Curl(const string &remote)
{
    string info;
    CURL *curl = curl_easy_init();
    if (curl == NULL)
    {
        std::cerr << "curl_easy_init return NULL !" << std::endl;
    }

    curl_easy_setopt(curl, CURLOPT_URL, remote.c_str());

    // 允许与服务器的连接阶段所花费的最长时间
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 1L);

    // 允许服务器在会话被认为是死的之前为一个命令发送一个响应信息的时间
    curl_easy_setopt(curl, CURLOPT_FTP_RESPONSE_TIMEOUT, 1L);

    // curl_easy_setopt(curl, CURLOPT_DIRLISTONLY, 1L);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "LIST");

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, GetFtpFileList);
    /* Set a pointer to our struct to pass to the callback */
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &info);

    /* 打开完整的协议/调试输出*/
    // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
        std::cerr << "curl_easy_perform(" << res << ") failed: " << curl_easy_strerror(res) << std::endl;
    else
        m_ListCurlMap.insert(ListCurlMap::value_type(remote, curl));
}

void PersistentList(CURL *curl)
{
    string info;
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &info);

    /* 打开完整的协议/调试输出*/
    // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
        std::cerr << "curl_easy_perform(" << res << ") failed: " << curl_easy_strerror(res) << std::endl;

    if (info.empty())
    {
        cerr << "info.empty()" << endl;
        return;
    }

    cerr << "info:[" << info << "]" << endl;
}

int main(void)
{
    // 查看目录要往文件夹后加/
    // 不持久连接的curl
    vector<string> filename_vec;
    vector<string> all_filename_vec;
    vector<string> remote_vec;
    string remote = "sftp://lws:luoweisong@127.0.0.1/data1/MR_MDT_csv/ZTE/MDT";
    remote += "/";
    // remote_vec.push_back(remote);
    // remote = "sftp://lws:luoweisong@127.0.0.1/data1/MR_MDT_csv/ZTE/MR";
    // remote += "/";
    // remote_vec.push_back(remote);

    // remote = "sftp://lws:luoweisong@127.0.0.1/data4/lws/demo";
    // remote += "/";
    // remote_vec.push_back(remote);
    // remote = "sftp://lws:luoweisong@127.0.0.1/data4/lws/demo/get-file-rand-x-y";
    // remote += "/";
    // remote_vec.push_back(remote);
    // remote = "sftp://lws:luoweisong@127.0.0.1/data4/lws/demo/get-file-rand-x-y/new-rule-files";
    // remote += "/";
    // remote_vec.push_back(remote);
    // remote = "sftp://lws:luoweisong@127.0.0.1/data4/lws/learning";
    // remote += "/";
    // remote_vec.push_back(remote);
    // remote = "sftp://lws:luoweisong@127.0.0.1/data4/lws/learning/New-features-of-C++11";
    // remote += "/";
    // remote_vec.push_back(remote);
    // remote = "sftp://lws:luoweisong@127.0.0.1/data4/lws/learning/New-features-of-C++11/atomic";
    // remote += "/";
    // remote_vec.push_back(remote);
    // remote = "sftp://lws:luoweisong@127.0.0.1/data4/lws/learning/New-features-of-C++11/condition_variable";
    // remote += "/";
    // remote_vec.push_back(remote);
    // remote = "sftp://lws:luoweisong@127.0.0.1/data4/lws/learning/New-features-of-C++11/emplace_back-and-push_back";
    // remote += "/";
    // remote_vec.push_back(remote);

    // for(int i=0; i<remote_vec.size(); i++)
    // {
    //     guint64 ls_time = GetCurrentMicroTime();
    //     ls(remote_vec[i], &filename_vec, &all_filename_vec);
    //     cerr << "ls Cost: " << GetCurrentMicroTime() - ls_time << " us." << endl;
    // }

    remote = "sftp://lws:luoweisong@127.0.0.1/home/lws";
    remote += "/";
    remote_vec.push_back(remote);

    // 持久连接的curl
    for (int i = 0; i < remote_vec.size(); i++)
    {
        if (m_ListCurlMap.find(remote_vec[i]) == m_ListCurlMap.end())
        {
            // cerr << "remote_vec[" << i << "] =  " << remote_vec[i] << endl;
            guint64 ls_time = GetCurrentMicroTime();
            ls_Init_Curl(remote_vec[i]);
            cerr << "ls_Init_Curl Cost: " << GetCurrentMicroTime() - ls_time << " us." << endl;
        }
    }
    cerr << "start to PersistentList..." << endl;
    ListCurlMap::iterator it = m_ListCurlMap.begin();
    // for(int i=0; i<100; i++)
    {
        while (it != m_ListCurlMap.end())
        {
            // cerr << "it->first = " << it->first << endl;
            guint64 ls_time = GetCurrentMicroTime();
            PersistentList(it->second);
            cerr << "PersistentList Cost: " << GetCurrentMicroTime() - ls_time << " us." << endl;
            it++;
        }
        usleep(1000000);
    }

    // cerr << "ls finish." << endl;

    // for(int i=0; i<all_filename_vec.size(); i++)
    // {
    //     string save_path = "./" + filename_vec[i];
    //     get(all_filename_vec[i].c_str(), save_path.c_str());
    // }

    return 0;
}