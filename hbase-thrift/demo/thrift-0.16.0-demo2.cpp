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

int main()
{
    std::shared_ptr<TTransport> socket(new TSocket("localhost", 9090));
    std::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    std::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    THBaseServiceClient client(protocol);
    try
    {
        transport->open();

        // do something

        transport->close();
    }
    catch (TException &tx)
    {
        printf("ERROR: %s\n", tx.what());
    }
}