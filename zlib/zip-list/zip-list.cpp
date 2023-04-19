#include "zip.h"
#include "unzip.h"
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <iterator>
#include <sstream>

using namespace std;

int main()
{
    // // 创建压缩流
    // zipFile zf=NULL;
    // zf = zipOpen64("test.zip", 0);
    // // 压缩文件
    // zip_fileinfo FileInfo;
    // memset(&FileInfo, 0, sizeof(FileInfo));
    // int level=Z_DEFAULT_COMPRESSION;
    // const char* password=NULL;
    // unsigned long crcFile=0;
    // // zipOpenNewFileInZip4(zf, "zip-list.cpp", &FileInfo, NULL, 0, NULL, 0, NULL, Z_DEFLATED, level, 0, -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, password, 0, 0, crcFile);
    // // // 注意buf为文件名，password为空时需要给NULL,level为压缩比，9为最高，0最低。

    // //比较简单的接口
    // zipOpenNewFileInZip(zf, "zip-list.cpp", &FileInfo, NULL, 0, NULL, 0, NULL, Z_DEFLATED, level);
    // std::ifstream fileHandle;
    // fileHandle.open("/data4/lws/demo/test-zlib/zip/zip-list.cpp");
    // std::string strFileBuf;
    // int nFileLen = 0;
    // if(fileHandle.is_open())
    // {
    //     std::stringstream buffer;
    //     buffer << fileHandle.rdbuf();
    //     strFileBuf = buffer.str();
    //     std::cout << strFileBuf << std::endl;
    // }
    // // 写入压缩文件
    // int ret = zipWriteInFileInZip(zf, (const void*)strFileBuf.c_str(), strFileBuf.size());
    // if(ret != ZIP_OK )
    // {
    //     zipCloseFileInZip(zf);
    //     zipClose(zf,NULL);
    //     return 0;
    // }
    // // 关闭当前文件
    // zipCloseFileInZip(zf);
    // // 关闭流
    // zipClose(zf,0);
    // return 1;

    unzFile unzip = NULL;
    unzip = unzOpen("./TD-LTE_MRO_ZTE_OMC1_183574_20220223120000.zip");
    if (NULL == unzip)
        return false;

    int done = unzGoToFirstFile(unzip);
    char szZipName[4096];
    memset(szZipName, 0, sizeof(szZipName));
    int file_num = 0;
    char buf[4096];
    memset(buf, 0, sizeof(buf));
    while (done == UNZ_OK)
    {
        unz_file_info file_info;
        unzGetCurrentFileInfo(unzip, &file_info, szZipName, sizeof(szZipName), NULL, 0, NULL, 0);
        // unz_file_pos file_pos;
        // unzGetFilePos(unzip, &file_pos);
        cerr << szZipName << endl;

        //找到一个指定的文件
        if (string(szZipName).find("211937_HUAWEI") != string::npos)
        {
            cerr << file_info.uncompressed_size << endl;
            unzOpenCurrentFilePassword(unzip, NULL);
            ofstream ofile;
            ofile.open((string("./") + szZipName).c_str());
            if (!ofile.is_open())
            {
                cerr << "open ./" << szZipName << " failed!" << endl;
                unzCloseCurrentFile(unzip);
                continue;
            }
            int len = 0;
            while ((len = unzReadCurrentFile(unzip, buf, sizeof(buf))) > 0)
            {
                // cerr << "len = " << len << endl;
                ofile.write(buf, len);
            }
            unzCloseCurrentFile(unzip);
            ofile.close();
        }
        done = unzGoToNextFile(unzip);
        file_num++;
    }
    cerr << "file_num = " << file_num << endl;
    unzClose(unzip);
}