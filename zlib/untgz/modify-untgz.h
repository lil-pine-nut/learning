#ifndef __MODIFY__UNTGZ__H__
#define __MODIFY__UNTGZ__H__

#include <string>
#include <vector>

/**
 * @brief 获取tar压缩文件的文件列表
 *
 * @param tar_file  输入压缩文件
 * @param list_vec  输出文件列表
 */
void TarList(const char *tar_file, std::vector<std::string> *list_vec);

/**
 * @brief 解压tar压缩文件
 *
 * @param tar_file      输入压缩文件
 * @param files_vec     输入压缩文件列表中需要解压的文件，为空则全部解压
 * @param unzip_path    输入解压至路径
 */
void TarUnzipFiles(const char *tar_file, std::vector<std::string> &files_vec, const std::string &unzip_path);

#endif