#include "modify-untgz.h"
#include <iostream>
#include <string.h>
#include "zlib.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

void help(int exitval)
{
    printf("untgz version 0.2.1\n"
           "  using zlib version %s\n\n",
           zlibVersion());
    printf("Usage: untgz file.tgz            extract all files\n"
           "       untgz file.tgz fname ...  extract selected files\n"
           "       untgz -l file.tgz         list archive contents\n"
           "       untgz -h                  display this help\n"
           "       untgz -d path file.tgz    specify output path extrac all files\n"
           "       untgz -d path file.tgz fname ... specify output path extrac selected files\n");
    exit(exitval);
}

char *TGZfname OF((const char *));
void TGZnotfound OF((const char *));

const char *TGZsuffix[] = {"\0", ".tar", ".tar.gz", ".taz", ".tgz", NULL};

/* return the file name of the TGZ archive */
/* or NULL if it does not exist */

char *TGZfname(const char *arcname)
{
    static char buffer[1024];
    int origlen, i;

    strcpy(buffer, arcname);
    origlen = strlen(buffer);

    for (i = 0; TGZsuffix[i]; i++)
    {
        strcpy(buffer + origlen, TGZsuffix[i]);
        if (access(buffer, F_OK) == 0)
            return buffer;
    }
    return NULL;
}

/* error message for the filename */

void TGZnotfound(const char *arcname)
{
    int i;

    fprintf(stderr, "Couldn't find ");
    for (i = 0; TGZsuffix[i]; i++)
        fprintf(stderr, (TGZsuffix[i + 1]) ? "%s%s, " : "or %s%s\n",
                arcname,
                TGZsuffix[i]);
    exit(1);
}

int main(int argc, char **argv)
{
    int action = -1; // 0 解压; 1, list
    int arg = 1;
    char *TGZfile;
    int unzip_file_num = 1;
    string unzip_path;

    if (argc == 1)
        help(0);

    if (strcmp(argv[arg], "-l") == 0)
    {
        action = 1;
        if (argc == ++arg)
            help(0);
    }
    else if (strcmp(argv[arg], "-h") == 0)
    {
        help(0);
    }
    else if (strcmp(argv[arg], "-d") == 0)
    {
        unzip_path = argv[2];
        action = 0;
        unzip_file_num = 4;
        arg += 2;
    }
    else if (argc >= 2)
    {
        action = 0;
        unzip_file_num = 2;
    }

    if ((TGZfile = TGZfname(argv[arg])) == NULL)
        TGZnotfound(argv[arg]);

    ++arg;

    if ((action == 1) && (arg != argc))
        help(1);

    /*
     *  Process the TGZ file
     */
    switch (action)
    {
    case 0:
    {
        vector<string> files_vec;
        for (int i = unzip_file_num; i < argc; i++)
        {
            files_vec.push_back(argv[i]);
        }
        for (int i = 0; i < files_vec.size(); i++)
        {
            cerr << "files_vec[" << i << "] = " << files_vec[i] << endl;
        }
        if (unzip_path.empty())
            unzip_path = "./";
        if (access(unzip_path.c_str(), 0) != F_OK)
        {
            cerr << "Directory:" << unzip_path << " Not Exit!" << endl;
            exit(0);
        }
        cerr << "unzip_path = " << unzip_path << endl;

        TarUnzipFiles(TGZfile, files_vec, unzip_path);
        break;
    }
    case 1:
    {
        vector<string> list_vec;

        TarList(TGZfile, &list_vec);
        for (int i = 0; i < list_vec.size(); i++)
        {
            // cerr << "list_vec[" << i << "] = " << list_vec[i] << endl;
            cerr << list_vec[i] << endl;
        }
        break;
    }

    default:
        cerr << ("Unknown option") << endl;
        exit(1);
    }

    return 0;
}