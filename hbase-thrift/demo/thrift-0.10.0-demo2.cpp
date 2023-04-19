#include <iostream>
#include <string>
#include <vector>
#include "THBaseService.h"
#include <transport/TSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>
using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::hadoop::hbase::thrift2;
using boost::shared_ptr;

int main()
{
    boost::shared_ptr<TTransport> socket(new TSocket("127.0.0.1", 9090));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    THBaseServiceClient client(protocol);
    try
    {
        transport->open();

        TPut put;
        string rowkey = "rowkey";
        string tableName = "table5555";
        vector<TColumnValue> cvs;
        TColumnValue colum;
        colum.__set_family("wa");
        colum.__set_qualifier("myKey");
        colum.__set_value("myValue");
        cvs.push_back(colum);
        put.__set_row(rowkey);
        put.__set_columnValues(cvs);
        client.put(tableName, put);

        transport->close();
    }
    catch (TException &tx)
    {
        printf("ERROR: %s\n", tx.what());
    }
}