#include <zlib.h>
#include <iostream>
#include <fstream>
#include "set_attributes.h"

using namespace std;

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        cerr << "usage: ./gunzip-test file.gz" << endl;
        return 0;
    }
    string gzip_file = argv[1];
    gzFile gf = gzopen(gzip_file.c_str(), "rb");
    if (gf == NULL)
    {
        cerr << "Can't gzopen " << gzip_file << endl;
        return 0;
    }

    //直接去掉最后三个字符
    string file = gzip_file.substr(0, gzip_file.size() - 3);
    ofstream ofile;
    ofile.open(file.c_str());
    if (!ofile.is_open())
    {
        cerr << "Can't open " << file << endl;
        return 0;
    }
    int err;
    int len;
    char buff[4096];
    while (1)
    {
        len = gzread(gf, buff, sizeof(buff));
        if (len < 0)
        {
            cerr << (gzerror(gf, &err)) << endl;
            return -1;
        }
        if (len == 0)
            break;
        ofile.write(buff, len);
    }
    gzclose(gf);
    ofile.close();

    //真的好吗？
    copymeta(gzip_file.c_str(), file.c_str());
}