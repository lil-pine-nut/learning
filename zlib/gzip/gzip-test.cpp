#include <zlib.h>
#include <iostream>
#include <fstream>
#include "set_attributes.h"

using namespace std;

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        cerr << "usage: ./gzip-test file" << endl;
        return 0;
    }
    string file = argv[1];
    ifstream ifile;
    ifile.open(file.c_str());
    if (!ifile.is_open())
    {
        cerr << "Can't open " << file << endl;
        return 0;
    }
    string gzip_file = file + ".gz";
    gzFile gf = gzopen(gzip_file.c_str(), "w");
    if (gf == NULL)
    {
        cerr << "Can't gzopen " << file << endl;
        return 0;
    }
    char buff[4096];
    int len;
    while (1)
    {
        len = ifile.readsome(buff, sizeof(buff));
        if (len <= 0)
            break;
        gzwrite(gf, buff, len);
    }
    gzclose(gf);
    ifile.close();

    //真的好吗？
    copymeta(file.c_str(), gzip_file.c_str());
}