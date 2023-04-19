/**
 *
 * This is a performance test class for HbCli class.
 *
 * Author:
 *   jiuling.ypf<jiuling.ypf@taobao.com>
 *
 * Date:
 *   2012-08-23
 *
 */
#include <iostream>
#include <ctime>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

#include "hbcli.h"

const int COUNT = 10000;

struct timeval tvpre, tvafter;

static char *rand_str(char *str, const int len)
{
    int i;
    for (i = 0; i < len; ++i)
        str[i] = 'A' + rand() % 26;
    str[++i] = '\0';
    return str;
}

static inline long gen_ms(timeval tvpre, timeval tvafter)
{
    return (tvafter.tv_sec - tvpre.tv_sec) * 1000 + (tvafter.tv_usec - tvpre.tv_usec) / 1000;
}

long test_row_put(HbCli myhbcli, std::string table, int klen, int vlen)
{
    long ms = 0;
    char key[klen + 1];
    char value[vlen + 1];
    for (int i = 0; i < COUNT; i++)
    {
        rand_str(key, klen);
        rand_str(value, vlen);
        gettimeofday(&tvpre, NULL);
        myhbcli.putRow(table, key, "entry:m", value);
        gettimeofday(&tvafter, NULL);
        ms += gen_ms(tvpre, tvafter);
    }
    return ms;
}

string itoa(int num)
{
    char buff[12] = {0};
    sprintf(buff, "%u", num);
    return buff;
}

long test_row_put_list(HbCli myhbcli, std::string table, int klen, int vlen, int lnum)
{
    long ms = 0;
    char key[klen + 1];
    char value[vlen + 1];
    RowMap rowMap;
    StrMap columnMap;
    for (int i = 0; i < COUNT; i++)
    {
        rand_str(key, klen);
        rand_str(value, vlen);
        columnMap.clear();
        for (int j = 0; j < 100; j++)
            columnMap.insert(std::pair<std::string, std::string>("entry:m" + itoa(j), value));
        rowMap.insert(std::pair<std::string, StrMap>(key, columnMap));
        if (rowMap.size() == COUNT)
        {
            gettimeofday(&tvpre, NULL);
            myhbcli.putRows(table, rowMap);
            rowMap.clear();
            gettimeofday(&tvafter, NULL);
            ms += gen_ms(tvpre, tvafter);
        }
    }
    return ms;
}

long test_row_put_list2(HbCli myhbcli, std::string table, int klen, int vlen, int lnum)
{

    long ms = 0;
    char key[klen + 1];
    char value[vlen + 1];
    vector<BatchMutation> rowBatches;
    vector<Mutation> mutations;
    for (int i = 0; i < COUNT; i++)
    {
        rand_str(key, klen);
        rand_str(value, vlen);
        mutations.clear();
        for (int j = 0; j < 100; j++)
        {
            mutations.push_back(Mutation());
            mutations.back().column = "entry:m" + itoa(j);
            mutations.back().value = value;
        }
        rowBatches.push_back(BatchMutation());
        rowBatches.back().row = key;
        rowBatches.back().mutations = mutations;
        if (rowBatches.size() == COUNT)
        {
            gettimeofday(&tvpre, NULL);
            myhbcli.putRows(table, rowBatches);
            rowBatches.clear();
            gettimeofday(&tvafter, NULL);
            ms += gen_ms(tvpre, tvafter);
        }
    }
    return ms;
}

int main(int argc, char **argv)
{
    if (argc < 6)
    {
        std::cerr << "Invalid arguments!\n"
                  << "Usage: testput host port key_len val_len list_num" << std::endl;
        return -1;
    }
    HbCli myhbcli(argv[1], atoi(argv[2]));

    myhbcli.connect();

    std::string table("test_table");
    ColVec columns;
    columns.push_back(ColumnDescriptor());
    columns.back().name = "entry:";
    columns.back().maxVersions = 1;
    columns.back().compression = "GZ";
    columns.back().inMemory = true;
    columns.back().blockCacheEnabled = true;
    columns.back().bloomFilterType = "ROW";
    columns.back().timeToLive = 3 * 24 * 3600;
    if (!myhbcli.tableExists(table))
        myhbcli.createTable(table, columns);

    int klen = atoi(argv[3]);
    int vlen = atoi(argv[4]);
    int lnum = atoi(argv[5]);
    long actual_ms = 0;
    struct timeval tvstart, tvend;
    cerr << "lnum : " << lnum << endl;
    gettimeofday(&tvstart, NULL);
    if (lnum == 1)
        actual_ms = test_row_put_list(myhbcli, table, klen, vlen, lnum);
    else
        actual_ms = test_row_put_list2(myhbcli, table, klen, vlen, lnum);
    gettimeofday(&tvend, NULL);
    long total_ms = gen_ms(tvstart, tvend);
    std::cout << "total time in ms: " << total_ms << std::endl;
    std::cout << "actual time in ms: " << actual_ms << std::endl;
    std::cout << "qps in total time: " << (long)(COUNT * 1000) / total_ms << std::endl;
    std::cout << "qps in actual time: " << (long)(COUNT * 1000) / actual_ms << std::endl;

    myhbcli.deleteTable(table);
    myhbcli.disconnect();
}
