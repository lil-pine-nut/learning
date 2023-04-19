#include <iostream>
#include <string>
#include <vector>
#include "Hbase.h"
#include <transport/TSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::hadoop::hbase::thrift;
using boost::shared_ptr;

int main()
{
    boost::shared_ptr<TTransport> socket(new TSocket("127.0.0.1", 9090));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    HbaseClient client(protocol);
    try
    {
        transport->open();
        printf("open success.\n");
        std::string table("test_table");
        std::vector<ColumnDescriptor> columns;
        columns.push_back(ColumnDescriptor());
        columns.back().name = "entry:";
        columns.back().maxVersions = 1;
        columns.back().compression = "GZ";
        columns.back().inMemory = true;
        columns.back().blockCacheEnabled = true;
        columns.back().bloomFilterType = "ROW";
        columns.back().timeToLive = 3 * 24 * 3600;

        // 判断table是否存在
        bool is_table_exit = false;
        std::vector<std::string> tables;
        client.getTableNames(tables);
        for (std::vector<std::string>::const_iterator it = tables.begin(); it != tables.end(); ++it)
        {
            if (table == *it)
            {
                is_table_exit = true;
                cerr << table << " exit." << endl;
                break;
            }
        }
        if (!is_table_exit)
        {
            std::cout << "creating table: " << table << std::endl;
            client.createTable(table, columns);
        }

        transport->close();
    }
    catch (TException &tx)
    {
        printf("ERROR: %s\n", tx.what());
    }
}