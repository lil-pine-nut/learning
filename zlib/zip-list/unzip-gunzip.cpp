#include "unzip.h"
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <iterator>
#include <sstream>
#include <vector>

using namespace std;

typedef unsigned long long guint64;
typedef unsigned int guint32;
typedef unsigned short guint16;
typedef unsigned char guint8;

#include <sys/time.h>
#include <unistd.h>
guint64 GetCurrentMicroTime()
{
    struct timeval dwStart;
    gettimeofday(&dwStart, NULL);
    guint64 dwTime = 1000000 * dwStart.tv_sec + dwStart.tv_usec;
    return dwTime;
}

inline string ClearGzipSuffixName(const string &gzip_file_name)
{
    return gzip_file_name.substr(0, gzip_file_name.size() - 3);
}

string GetCompressedFilesSuffixName(const char *path_name)
{
    char *p = rindex((char *)path_name, '/');
    if (p)
        p = rindex(p, '.');
    else
        p = rindex((char *)path_name, '.');
    if (p)
        return string(p);
    else
        return string();
}

string GetCompressedFilesPath(const char *path_name)
{
    char *p = rindex((char *)path_name, '/');
    if (p)
        return string(path_name, p - path_name + 1);
    else
        return string();
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        cerr << "Usage: ./bin zip-file-path unzip-path" << endl;
        exit(0);
    }
    remove("/memfs/TD-LTE_MRO_ZTE_OMC1_804237_20220223120000.xml");
    string zip_file = argv[1];
    string unzip_path = argv[2];
    if (access(zip_file.c_str(), 0) != F_OK)
    {
        cerr << "Zip File:" << zip_file << " Not Exit!" << endl;
        exit(0);
    }

    if (access(unzip_path.c_str(), 0) != F_OK)
    {
        cerr << "Unzip Path:" << unzip_path << " Not Exit!" << endl;
        exit(0);
    }

    guint64 start = GetCurrentMicroTime();
    unzFile unzip = NULL;
    unzip = unzOpen(zip_file.c_str());
    if (NULL == unzip)
        return false;

    int done = unzGoToFirstFile(unzip);
    char szZipName[4096];
    memset(szZipName, 0, sizeof(szZipName));
    int file_num = 0;
    char buf[8192];
    memset(buf, 0, sizeof(buf));
    vector<string> middle_zip_vec;
    unzip_path += "/";
    string middle_zip_file;
    ofstream ofile;
    int len = 0;
    unz_file_info file_info;
    while (done == UNZ_OK)
    {
        unzGetCurrentFileInfo(unzip, &file_info, szZipName, sizeof(szZipName), NULL, 0, NULL, 0);
        // cerr << szZipName << endl;
        // if(string(szZipName) == "TD-LTE_MRO_ZTE_OMC1_87804_20220223120000.zip")
        {
            unzOpenCurrentFile(unzip);
            middle_zip_file = unzip_path + szZipName;
            // ofile.open(middle_zip_file.c_str());
            // if(!ofile.is_open())
            // {
            //     cerr << "Open" << middle_zip_file << " failed!" << endl;
            //     unzCloseCurrentFile(unzip);
            //     exit(0);
            // }
            // middle_zip_vec.push_back(middle_zip_file);
            // cerr << "middle_zip_file:" << middle_zip_file << endl;
            while ((len = unzReadCurrentFile(unzip, buf, sizeof(buf))) > 0)
            {
                // ofile.write(buf, len);
            }
            unzCloseCurrentFile(unzip);
            // ofile.close();
            file_num++;
            break;
        }
        done = unzGoToNextFile(unzip);
    }
    // cerr << "file_num = "<< file_num << endl;
    unzClose(unzip);

    cerr << "Unzip Cost:" << GetCurrentMicroTime() - start << "us." << endl;
    return -1;

    if (middle_zip_vec.empty())
        return 0;

    string suffix = GetCompressedFilesSuffixName(middle_zip_vec[0].c_str());
    if (suffix == ".gz")
    {
        string gunzip_file;
        gzFile gf = NULL;
        int err;
        for (size_t i = 0; i < middle_zip_vec.size(); i++)
        {
            gf = gzopen(middle_zip_vec[i].c_str(), "rb");
            if (gf == NULL)
            {
                cerr << "Can't gzopen " << middle_zip_vec[i] << endl;
                return 0;
            }
            gunzip_file = ClearGzipSuffixName(middle_zip_vec[i]);
            ofile.open(gunzip_file.c_str());
            if (!ofile.is_open())
            {
                cerr << "Can't open " << gunzip_file << endl;
                return 0;
            }
            while (1)
            {
                len = gzread(gf, buf, sizeof(buf));
                if (len < 0)
                {
                    cerr << (gzerror(gf, &err)) << endl;
                    return -1;
                }
                if (len == 0)
                    break;
                ofile.write(buf, len);
            }
            gzclose(gf);
            ofile.close();
            remove(middle_zip_vec[i].c_str());
        }
    }
    else
    {
        string gunzip_file;
        for (size_t i = 0; i < middle_zip_vec.size(); i++)
        {
            unzip = unzOpen(middle_zip_vec[i].c_str());
            if (NULL == unzip)
                return false;
            done = unzGoToFirstFile(unzip);
            while (done == UNZ_OK)
            {
                unzGetCurrentFileInfo(unzip, &file_info, szZipName, sizeof(szZipName), NULL, 0, NULL, 0);
                unzOpenCurrentFile(unzip);
                gunzip_file = GetCompressedFilesPath(middle_zip_vec[i].c_str()) + szZipName;
                ofile.open(gunzip_file.c_str());
                if (!ofile.is_open())
                {
                    cerr << "Open" << gunzip_file << " failed!" << endl;
                    unzCloseCurrentFile(unzip);
                    exit(0);
                }
                // cerr << "middle_zip_file:" << middle_zip_file << endl;

                while ((len = unzReadCurrentFile(unzip, buf, sizeof(buf))) > 0)
                {
                    // cerr << "len = " << len << endl;
                    ofile.write(buf, len);
                }
                unzCloseCurrentFile(unzip);
                ofile.close();
                done = unzGoToNextFile(unzip);
            }
            unzClose(unzip);
            remove(middle_zip_vec[i].c_str());
        }
    }

    cerr << "Gunzip Cost:" << GetCurrentMicroTime() - start << "us." << endl;
}