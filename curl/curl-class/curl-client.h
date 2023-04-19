/**
 * @file curl-client.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2021-12-29
 * @reference https://curl.se/libcurl/c/example.html and  https://curl.se/libcurl/c/curl_easy_setopt.html
 *
 *
 * @copyright Copyright (c) 2021
 *
 */

#ifndef CURL_CLIENT_H
#include "curl/curl.h"
#include <iostream>
#include <string>

class CurlClient
{

public:
    enum PROTOCOL
    {
        FTP,
        SFTP,
        SCP
    };

    CurlClient(/* args */);
    ~CurlClient();

    /**
     * @brief 连接
     *
     * @param strUrl            url或完整的IP和端口
     * @param strLogin          登录名
     * @param strPassword       密码
     * @param public_key_path   ssh的public key路径
     * @param Protocol          FTP或SFTP
     * @param password_auth     是否是密码认证的方法，是：密码；否：ssh的public key。注意：FTP只支持密码认证
     * @return true
     * @return false
     */
    bool connect(const char *strUrl, const char *strLogin, const char *strPassword,
                 const char *public_key_path, const PROTOCOL &Protocol, bool password_auth = true,
                 unsigned int connect_timeout = 300, unsigned int ftp_response_timeout = 300);

    /**
     * @brief 上传文件
     *
     * @param local_file_path   本地文件路径
     * @param remote_file_path  服务器的文件路径
     * @return true
     * @return false
     */
    bool put(const char *local_file_path, const char *remote_file_path);

    /**
     * @brief 重命名文件
     *
     * @param file_path         服务器的文件路径
     * @param rename_file_path  服务器重命名后的文件路径
     * @return true
     * @return false
     */
    bool mv(const char *file_path, const char *rename_file_path);

    /**
     * @brief 获取文件
     *
     * @param remote_file_path  服务器的文件路径
     * @param local_file_path   本地的文件路径
     * @return true
     * @return false
     */
    bool get(const char *remote_file_path, const char *local_file_path);

    /**
     * @brief 查看服务器列表，SCP只能ls文件。注意：查看目录要往文件夹路径后加 /
     *
     * @param remote_file_path  服务器的文件路径
     * @param info              列表信息
     * @return true
     * @return false
     */
    bool ls(const char *remote_file_path, std::string &info);

    /**
     * @brief 创建服务器的文件夹
     *
     * @param remote_file_path 服务器的文件路径
     * @return true
     * @return false
     */
    bool mkdir(const char *remote_file_path);

private:
    // 查看目录要往文件夹路径后加/
    bool ListDirectory(const char *remote_file_path, std::string &info);

    /*
        TODO： Windows7用FreeSSHd开启的SFTP服务器，无论是否有该文件夹判断都失败了.
        在centOS-7.6.1810上SFTP支持，且文件夹后不能带"/".
        不稳定，不建议使用。
    */
    bool IsDirectory(const char *remote_file_path);

    /**
     * @brief 解析URL路径
     *
     * @param strRemoteFile 服务器的路径
     * @return std::string
     */
    std::string ParseURL(const char *strRemoteFile) const;

    /**
     * @brief 把字符串替换，主要是为了解决windows单"/"的错误
     *
     * @param strSubject    完整的字符串
     * @param strSearch     需要替换的字符串
     * @param strReplace    替换后的字符串
     */
    static void ReplaceString(std::string &strSubject, const std::string &strSearch, const std::string &strReplace);

private:
    std::string m_strUserName_;     //用户名
    std::string m_strPassword_;     //密码
    std::string m_strServerUrl_;    // url
    std::string m_public_key_path_; // ssh的public key路径
    PROTOCOL m_protocol_;           //协议：FTP或SFTP
    bool m_password_auth_;          //是否是密码认证的方法，是：密码；否：ssh的public key

    unsigned int m_connect_timeout;      //连接超时
    unsigned int m_ftp_response_timeout; // ftp响应超时
};

#endif