#include "curl-client.h"
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <algorithm>
#include <fstream>

CurlClient::CurlClient(/* args */)
{
    m_password_auth_ = true;
    m_connect_timeout = 0;
    m_ftp_response_timeout = 0;
}

CurlClient::~CurlClient()
{
    curl_global_cleanup();
}

bool CurlClient::connect(const char *strUrl, const char *strLogin, const char *strPassword,
                         const char *public_key_path, const PROTOCOL &Protocol, bool password_auth,
                         unsigned int connect_timeout, unsigned int ftp_response_timeout)
{

    if (strUrl != NULL)
    {
        m_strServerUrl_ = strUrl;
    }
    else
    {
        return false;
    }

    if (strLogin != NULL)
    {
        m_strUserName_ = strLogin;
    }

    if (strPassword != NULL)
    {
        m_strPassword_ = strPassword;
    }

    if (public_key_path != NULL)
    {
        m_public_key_path_ = public_key_path;
    }

    m_protocol_ = Protocol;
    m_password_auth_ = password_auth;

    m_connect_timeout = connect_timeout;
    m_ftp_response_timeout = ftp_response_timeout;

    return true;
}

#include <sstream>
std::string DoubleToString(double d)
{
    std::string str;
    std::stringstream ss;
    ss << d;
    ss >> str;
    return str;
}

size_t ReadFromFileCallback(void *ptr, size_t size, size_t nmemb, void *stream)
{
    std::ifstream *pFileStream = reinterpret_cast<std::ifstream *>(stream);
    if (pFileStream->is_open())
    {
        pFileStream->read(reinterpret_cast<char *>(ptr), size * nmemb);
        return pFileStream->gcount();
    }
    return 0;
}

size_t WriteFileCallback(void *ptr, size_t size, size_t nmemb, void *stream)
{
    std::ofstream *pFileStream = reinterpret_cast<std::ofstream *>(stream);
    if (pFileStream->is_open())
    {
        pFileStream->write(reinterpret_cast<char *>(ptr), size * nmemb);
        return size * nmemb;
    }
    return 0;
}

int getFTPFileList(void *ptr, size_t size, size_t nmemb, void *stream)
{
    std::string &buffer = *(static_cast<std::string *>(stream));
    char *pBuf = (char *)ptr;
    size_t len = size * nmemb;
    for (size_t i = 0; i < len; ++i)
    {
        buffer += *pBuf;
        ++pBuf;
    }
    return len;
}

size_t throw_away(void *ptr, size_t size, size_t nmemb, void *data)
{
    (void)ptr;
    (void)data;
    /* we are not interested in the headers itself,
       so we only return the size we would have saved ... */
    return (size_t)(size * nmemb);
}

void CurlClient::ReplaceString(std::string &strSubject, const std::string &strSearch, const std::string &strReplace)
{
    if (strSearch.empty())
        return;

    size_t pos = 0;
    while ((pos = strSubject.find(strSearch, pos)) != std::string::npos)
    {
        strSubject.replace(pos, strSearch.length(), strReplace);
        pos += strReplace.length();
    }
}

std::string CurlClient::ParseURL(const char *strRemoteFile) const
{
    std::string strURL = m_strServerUrl_ + "/" + strRemoteFile;

    ReplaceString(strURL, "/", "//");

    std::string strUri = strURL;

    std::transform(strUri.begin(), strUri.end(), strUri.begin(), ::toupper);

    if (strUri.compare(0, 3, "FTP") != 0 && strUri.compare(0, 4, "SFTP") != 0)
    {
        switch (m_protocol_)
        {
        case FTP:
        default:
            strURL = "ftp://" + strURL;
            break;

        case SFTP:
            strURL = "sftp://" + strURL;
            break;

        case SCP:
            strURL = "scp://" + strURL;
            break;
        }
    }

    return strURL;
}

bool CurlClient::put(const char *local_file_path, const char *remote_file_path)
{
    if (local_file_path == NULL || remote_file_path == NULL)
    {
        return false;
    }

    struct stat file_info;

    /* 获得上传文件的大小 */
    if (stat(local_file_path, &file_info))
    {
        std::cerr << "Couldn't open '" << local_file_path << "' : " << strerror(errno) << std::endl;
        return false;
    }
    std::ifstream filestream;
    filestream.open(local_file_path, std::ifstream::in | std::ifstream::binary);
    if (!filestream)
    {
        return false;
    }

    std::string strLocalRemoteFile = ParseURL(remote_file_path);

    CURL *curl = curl_easy_init();
    if (curl)
    {
        /* specify target */
        curl_easy_setopt(curl, CURLOPT_URL, strLocalRemoteFile.c_str());

        /* we want to use our own read function */
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, ReadFromFileCallback);

        /* now specify which file to upload */
        curl_easy_setopt(curl, CURLOPT_READDATA, &filestream);

        /* Set the size of the file to upload (optional).  If you give a *_LARGE
        option you MUST make sure that the type of the passed-in argument is a
        curl_off_t. If you use CURLOPT_INFILESIZE (without _LARGE) you must
        make sure that to pass in a type 'long' argument. */
        curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, static_cast<curl_off_t>(file_info.st_size));

        /* enable uploading */
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

        curl_easy_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS, CURLFTP_CREATE_DIR);

        if (m_password_auth_)
        {
            curl_easy_setopt(curl, CURLOPT_SSH_AUTH_TYPES, CURLSSH_AUTH_PASSWORD);
            if (!m_strUserName_.empty() || !m_strPassword_.empty())
                curl_easy_setopt(curl, CURLOPT_USERPWD, (m_strUserName_ + ":" + m_strPassword_).c_str());
        }
        else
        {
            curl_easy_setopt(curl, CURLOPT_SSH_AUTH_TYPES, CURLSSH_AUTH_PUBLICKEY);
            if (!m_public_key_path_.empty())
                curl_easy_setopt(curl, CURLOPT_SSH_PUBLIC_KEYFILE, m_public_key_path_.c_str());
        }

        /* 打开完整的协议/调试输出*/
        // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        /* 运行 */
        CURLcode res = curl_easy_perform(curl);

        /* 容错处理 */
        if (res != CURLE_OK)
        {
            std::cerr << "curl_easy_perform(" << res << ") failed: " << curl_easy_strerror(res) << std::endl;

            /* always cleanup */
            curl_easy_cleanup(curl);
            filestream.close();
            return false;
        }
        // else {
        //     /* now extract transfer info */
        //     curl_off_t speed_upload, total_time;
        //     curl_easy_getinfo(curl, CURLINFO_SPEED_UPLOAD_T, &speed_upload);
        //     curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME_T, &total_time);

        //     fprintf(stderr, "Speed: %" CURL_FORMAT_CURL_OFF_T " bytes/sec during %"
        //             CURL_FORMAT_CURL_OFF_T ".%06ld seconds\n",
        //             speed_upload,
        //             (total_time / 1000000), (long)(total_time % 1000000));

        // }

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    else
    {
        return false;
    }

    filestream.close();

    return true;
}

bool CurlClient::IsDirectory(const char *remote_file_path)
{
    CURL *curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, remote_file_path);
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);

    if (m_password_auth_)
    {
        curl_easy_setopt(curl, CURLOPT_SSH_AUTH_TYPES, CURLSSH_AUTH_PASSWORD);
        if (!m_strUserName_.empty() || !m_strPassword_.empty())
            curl_easy_setopt(curl, CURLOPT_USERPWD, (m_strUserName_ + ":" + m_strPassword_).c_str());
        // curl_easy_setopt(curl, CURLOPT_USERNAME, m_strUserName_.c_str());
        // curl_easy_setopt(curl, CURLOPT_PASSWORD, m_strPassword_.c_str());
    }
    else
    {
        curl_easy_setopt(curl, CURLOPT_SSH_AUTH_TYPES, CURLSSH_AUTH_PUBLICKEY);
        if (!m_public_key_path_.empty())
            curl_easy_setopt(curl, CURLOPT_SSH_PUBLIC_KEYFILE, m_public_key_path_.c_str());
    }

    CURLcode res = curl_easy_perform(curl);
    if (res == CURLE_OK)
    {
        std::cout << "FOLDER EXISTS\n";
        curl_easy_cleanup(curl);
        return true;
    }
    else
    {
        std::cout << "FOLDER DOESN'T EXIST\n";
        // std::cerr<<"curl_easy_perform("<<res<<") failed: "<<curl_easy_strerror(res)<<std::endl;
        curl_easy_cleanup(curl);
        return false;
    }
}

bool CurlClient::mv(const char *file_path, const char *rename_file_path)
{
    if (file_path == NULL || rename_file_path == NULL)
    {
        return false;
    }

    std::string strLocalRemoteFile = ParseURL("");
    std::string RenameFrom;
    std::string RenameFrom2;
    if (m_protocol_ == FTP)
    {
        RenameFrom = std::string("RNFR ") + file_path;
        RenameFrom2 = std::string("RNTO ") + rename_file_path;
    }
    else if (m_protocol_ == SFTP)
    {
        //
        RenameFrom = "rename \"";
        RenameFrom.append(file_path).append("\" \"");
        RenameFrom.append(rename_file_path).append("\"");
    }
    std::cout << RenameFrom << std::endl;
    CURL *curl = curl_easy_init();
    if (curl)
    {
        struct curl_slist *quotelist = NULL;
        quotelist = curl_slist_append(quotelist, RenameFrom.c_str());
        if (!RenameFrom2.empty())
            quotelist = curl_slist_append(quotelist, RenameFrom2.c_str());
        /* specify target */
        curl_easy_setopt(curl, CURLOPT_URL, strLocalRemoteFile.c_str());
        /* pass in that last of FTP commands to run after the transfer */
        curl_easy_setopt(curl, CURLOPT_POSTQUOTE, quotelist);
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1L); //把打印消息取消
        curl_easy_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS, CURLFTP_CREATE_DIR);

        if (m_password_auth_)
        {
            curl_easy_setopt(curl, CURLOPT_SSH_AUTH_TYPES, CURLSSH_AUTH_PASSWORD);
            if (!m_strUserName_.empty() || !m_strPassword_.empty())
                curl_easy_setopt(curl, CURLOPT_USERPWD, (m_strUserName_ + ":" + m_strPassword_).c_str());
        }
        else
        {
            curl_easy_setopt(curl, CURLOPT_SSH_AUTH_TYPES, CURLSSH_AUTH_PUBLICKEY);
            if (!m_public_key_path_.empty())
                curl_easy_setopt(curl, CURLOPT_SSH_PUBLIC_KEYFILE, m_public_key_path_.c_str());
        }

        // curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, 100L);//接收数据时超时设置，如果10毫秒内数据未接收完，直接退出
        /* 打开完整的协议/调试输出*/
        // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        /* 运行 */
        CURLcode res = curl_easy_perform(curl);

        /* 容错处理 */
        if (res != CURLE_OK)
        {
            std::cerr << "curl_easy_perform(" << res << ") failed: " << curl_easy_strerror(res) << std::endl;

            /* clean up the FTP commands list */
            curl_slist_free_all(quotelist);

            /* always cleanup */
            curl_easy_cleanup(curl);

            return false;
        }

        /* clean up the FTP commands list */
        curl_slist_free_all(quotelist);

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    else
    {
        return false;
    }
    return false;
}

bool CurlClient::get(const char *remote_file_path, const char *local_file_path)
{
    if (remote_file_path == NULL || local_file_path == NULL)
    {
        return false;
    }

    std::string strLocalRemoteFile = ParseURL(remote_file_path);

    std::ofstream filestream;
    filestream.open(local_file_path, std::ifstream::out | std::ifstream::binary);
    if (!filestream)
    {
        return false;
    }

    CURL *curl = curl_easy_init();
    if (curl)
    {
        /* specify target */
        curl_easy_setopt(curl, CURLOPT_URL, strLocalRemoteFile.c_str());

        /* 定义回调函数，以便在需要写入数据时进行调用 */
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteFileCallback);

        /*设置一个指向我们的结构的指针传递给回调函数*/
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &filestream);

        if (m_password_auth_)
        {
            curl_easy_setopt(curl, CURLOPT_SSH_AUTH_TYPES, CURLSSH_AUTH_PASSWORD);
            if (!m_strUserName_.empty() || !m_strPassword_.empty())
                curl_easy_setopt(curl, CURLOPT_USERPWD, (m_strUserName_ + ":" + m_strPassword_).c_str());
        }
        else
        {
            curl_easy_setopt(curl, CURLOPT_SSH_AUTH_TYPES, CURLSSH_AUTH_PUBLICKEY);
            if (!m_public_key_path_.empty())
                curl_easy_setopt(curl, CURLOPT_SSH_PUBLIC_KEYFILE, m_public_key_path_.c_str());
        }

        /* 打开完整的协议/调试输出*/
        // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        /* 运行 */
        CURLcode res = curl_easy_perform(curl);

        /* 容错处理 */
        if (res != CURLE_OK)
        {
            std::cerr << "curl_easy_perform(" << res << ") failed: " << curl_easy_strerror(res) << std::endl;

            filestream.close();
            /* always cleanup */
            curl_easy_cleanup(curl);
            return false;
        }

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    else
    {
        return false;
    }

    filestream.close();
    return true;
}

bool CurlClient::ListDirectory(const char *remote_file_path, std::string &info)
{
    CURL *curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, remote_file_path);

    // curl_easy_setopt(curl, CURLOPT_DIRLISTONLY, 1L);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "LIST");

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, getFTPFileList);
    /* Set a pointer to our struct to pass to the callback */
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &info);

    if (m_password_auth_)
    {
        curl_easy_setopt(curl, CURLOPT_SSH_AUTH_TYPES, CURLSSH_AUTH_PASSWORD);
        if (!m_strUserName_.empty() || !m_strPassword_.empty())
            curl_easy_setopt(curl, CURLOPT_USERPWD, (m_strUserName_ + ":" + m_strPassword_).c_str());
    }
    else
    {
        curl_easy_setopt(curl, CURLOPT_SSH_AUTH_TYPES, CURLSSH_AUTH_PUBLICKEY);
        if (!m_public_key_path_.empty())
            curl_easy_setopt(curl, CURLOPT_SSH_PUBLIC_KEYFILE, m_public_key_path_.c_str());
    }

    CURLcode res = curl_easy_perform(curl);
    if (res == CURLE_OK)
    {
        curl_easy_cleanup(curl);
        return true;
    }
    else
    {
        curl_easy_cleanup(curl);
        return false;
    }
}

bool CurlClient::ls(const char *remote_file_path, std::string &info)
{
    if (remote_file_path == NULL)
    {
        return false;
    }

    std::string strLocalRemoteFile = ParseURL(remote_file_path);
    std::cerr << "strLocalRemoteFile = " << strLocalRemoteFile << std::endl;
    info.clear();

    CURL *curl = curl_easy_init();
    if (curl)
    {
        /* specify target */
        curl_easy_setopt(curl, CURLOPT_URL, strLocalRemoteFile.c_str());

        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5); //连接超时
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15);       //允许libcurl传输操作进行的最长时间，操作超时

        curl_easy_setopt(curl, CURLOPT_DIRLISTONLY, 1L);

        /* No download if the file */
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
        /* Ask for filetime */
        curl_easy_setopt(curl, CURLOPT_FILETIME, 1L);
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, throw_away);
        curl_easy_setopt(curl, CURLOPT_HEADER, 0L);

        if (m_password_auth_)
        {
            curl_easy_setopt(curl, CURLOPT_SSH_AUTH_TYPES, CURLSSH_AUTH_PASSWORD);
            if (!m_strUserName_.empty() || !m_strPassword_.empty())
                curl_easy_setopt(curl, CURLOPT_USERPWD, (m_strUserName_ + ":" + m_strPassword_).c_str());
        }
        else
        {
            curl_easy_setopt(curl, CURLOPT_SSH_AUTH_TYPES, CURLSSH_AUTH_PUBLICKEY);
            if (!m_public_key_path_.empty())
                curl_easy_setopt(curl, CURLOPT_SSH_PUBLIC_KEYFILE, m_public_key_path_.c_str());

            curl_easy_setopt(curl, CURLOPT_SSH_PRIVATE_KEYFILE, "/home/lws/.ssh/id_rsa");
        }

        // /* 打开完整的协议/调试输出*/
        // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        /* 运行 */
        CURLcode res = curl_easy_perform(curl);

        /* 容错处理 */
        if (res == CURLE_OK)
        {
            long filetime = -1;
            double filesize = 0.0;
            res = curl_easy_getinfo(curl, CURLINFO_FILETIME, &filetime);
            if ((CURLE_OK == res) && (filetime >= 0))
            {
                time_t file_time = (time_t)filetime;

                struct tm *local_time = NULL;
                local_time = localtime(&file_time);
                char buffer[30];
                memset(buffer, 0, sizeof(buffer));
                local_time = localtime(&file_time);
                int length = strftime(buffer, 80, "%h %d %H:%M\n", local_time);

                // printf("buffer = %s", buffer);
                buffer[strlen(buffer) - 1] = ' ';
                info.append(buffer);
            }
            res = curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD,
                                    &filesize);

            if ((CURLE_OK == res) && (filesize > 0.0))
            {
                info = DoubleToString(filesize) + " " + info + remote_file_path;
            }
            if (info.length() < 15)
            {
                info.clear();
                ListDirectory(strLocalRemoteFile.c_str(), info);
            }
        }
        else
        {
            std::cerr << "curl_easy_perform(" << res << ") failed: " << curl_easy_strerror(res) << std::endl;
            return false;
        }

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    else
    {
        return false;
    }
    return true;
}

bool CurlClient::mkdir(const char *remote_file_path)
{

    if (remote_file_path == NULL)
    {
        return false;
    }

    std::string strLocalRemoteFile = ParseURL("");

    CURL *curl = curl_easy_init();
    if (curl)
    {
        std::string strBuf;
        if (m_protocol_ == SFTP)
            strBuf = "mkdir ";
        else if (m_protocol_ == FTP)
            strBuf = "MKD ";
        strBuf.append(remote_file_path);
        struct curl_slist *quotelist = NULL;
        quotelist = curl_slist_append(quotelist, strBuf.c_str());
        curl_easy_setopt(curl, CURLOPT_URL, strLocalRemoteFile.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTQUOTE, quotelist);
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
        curl_easy_setopt(curl, CURLOPT_HEADER, 1L);
        curl_easy_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS, CURLFTP_CREATE_DIR);

        if (m_password_auth_)
        {
            curl_easy_setopt(curl, CURLOPT_SSH_AUTH_TYPES, CURLSSH_AUTH_PASSWORD);
            if (!m_strUserName_.empty() || !m_strPassword_.empty())
                curl_easy_setopt(curl, CURLOPT_USERPWD, (m_strUserName_ + ":" + m_strPassword_).c_str());
        }
        else
        {
            curl_easy_setopt(curl, CURLOPT_SSH_AUTH_TYPES, CURLSSH_AUTH_PUBLICKEY);
            if (!m_public_key_path_.empty())
                curl_easy_setopt(curl, CURLOPT_SSH_PUBLIC_KEYFILE, m_public_key_path_.c_str());
        }

        /* 打开完整的协议/调试输出*/
        // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        /* 运行 */
        CURLcode res = curl_easy_perform(curl);

        /* 容错处理 */
        if (res != CURLE_OK)
        {
            std::cerr << "curl_easy_perform(" << res << ") failed: " << curl_easy_strerror(res) << std::endl;

            curl_slist_free_all(quotelist);

            /* always cleanup */
            curl_easy_cleanup(curl);
            return false;
        }
        curl_slist_free_all(quotelist);

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    else
    {
        return false;
    }
    return true;
}