/**
 * Source: https://github.com/ypf412/hbase-thrift
 * Based on thrift-0.10.0 and hbase-1.3.0-src
 */

#ifndef __HBCLI__H__
#define __HBCLI__H__

#include <iostream>
#include <vector>

#include <protocol/TBinaryProtocol.h>
#include <transport/TSocket.h>
#include <transport/TTransportUtils.h>

#include "Hbase.h"

#include <sys/time.h>
#include <unistd.h>
typedef unsigned long long guint64;
typedef unsigned int guint32;
typedef unsigned short guint16;
typedef unsigned char guint8;
inline guint64 GetCurrentMicroTime()
{
    struct timeval dwStart;
    gettimeofday(&dwStart, NULL);
    guint64 dwTime = 1000000 * dwStart.tv_sec + dwStart.tv_usec;
    return dwTime;
}

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::hadoop::hbase::thrift;

typedef vector<string> StrVec;
typedef map<string, string> StrMap;
typedef vector<ColumnDescriptor > ColVec;
typedef map<string, ColumnDescriptor > ColMap;
typedef map<string, TCell > CellMap;
typedef vector<TRowResult> ResVec;
typedef map<string, map<string, string> > RowMap;

class HbCli
{
public:
    // Constructor and Destructor
    HbCli(const char *server, int port);
    ~HbCli();

    // Util Functions
    bool connect();
    bool disconnect();
    bool reconnect();
    inline bool isconnect()
    {
        return m_is_connected;
    }

    // HBase DDL Functions
    bool createTable(const string &table, const ColVec &columns);
    bool deleteTable(const string &table);
    bool tableExists(const string &table);

    // HBase DML Functions
    bool putRow(const string &table, const string &row, const string &column, const string &value);
    bool putRowWithColumns(const string &table, const string &row, const StrMap &columns);
    bool putRows(const string &table, const RowMap &rows);
    bool putRows(const string &table, const vector<BatchMutation> &rowBatches);
    bool getRow(const string &table, const string &row, ResVec &rowResult);
    bool getRowWithColumns(const string &table, const string &row, const StrVec &columns, ResVec &rowResult);
    bool getRows(const string &table, const StrVec &rows, ResVec &rowResult);
    bool getRowsWithColumns(const string &table, const StrVec &rows, const StrVec &columns, ResVec &rowResult);
    bool delRow(const string &table, const string &row);
    bool delRowWithColumn(const string &table, const string &row, const string &column);
    bool delRowWithColumns(const string &table, const string &row, const StrVec &columns);
    bool scan(const string &table, const string &startRow, StrVec &columns, ResVec &values);
    bool scanWithStop(const string &table, const string &startRow, const string &stopRow, StrVec &columns, ResVec &values);

    bool getTableRegions(std::vector<TRegionInfo> &_return, const Text &tableName);
    // HBase Util Functions
    void printRow(const ResVec &rowResult);

private:
    boost::shared_ptr<TTransport> m_socket;
    boost::shared_ptr<TTransport> m_transport;
    boost::shared_ptr<TProtocol> m_protocol;
    HbaseClient m_client;
    bool m_is_connected;
    map<Text, Text> m_attributes;
};
#endif // HBCLI_H_
