/**
 * Source: https://github.com/ypf412/hbase-thrift
 * Based on thrift-0.10.0 and hbase-1.3.0-src
 */

#include "hbcli.h"

HbCli::HbCli(const char *server, int port) : m_socket(new TSocket(server, port)),
                                             m_transport(new TBufferedTransport(m_socket)),
                                             m_protocol(new TBinaryProtocol(m_transport)),
                                             m_client(m_protocol)
{
    m_is_connected = false;
}

HbCli::~HbCli()
{
}

bool HbCli::connect()
{
    m_is_connected = false;
    try
    {
        m_transport->open();
        m_is_connected = true;
    }
    catch (const TException &tx)
    {
        cerr << "ERROR: " << tx.what() << endl;
    }
    return m_is_connected;
}

bool HbCli::disconnect()
{
    try
    {
        m_transport->close();
        m_is_connected = false;
        return true;
    }
    catch (const TException &tx)
    {
        cerr << "ERROR: " << tx.what() << endl;
        return false;
    }
}

bool HbCli::reconnect()
{
    bool flag = disconnect();
    flag = flag & connect();
    return flag;
}

bool HbCli::createTable(const string &table, const ColVec &columns)
{
    if (!isconnect())
    {
        return false;
    }
    try
    {
        cout << "creating table: " << table << endl;
        m_client.createTable(table, columns);
        return true;
    }
    catch (const AlreadyExists &ae)
    {
        cerr << "WARN: " << ae.message << endl;
    }
    catch (const TTransportException &tte)
    {
        cerr << "ERROR: " << tte.what() << endl;
        m_is_connected = false;
    }
    return false;
}

bool HbCli::deleteTable(const string &table)
{
    if (!isconnect())
    {
        return false;
    }
    bool exist = tableExists(table);
    if (exist)
    {
        try
        {
            if (m_client.isTableEnabled(table))
            {
                cout << "disabling table: " << table << endl;
                m_client.disableTable(table);
            }
            cout << "deleting table: " << table << endl;
            m_client.deleteTable(table);
            return true;
        }
        catch (const TTransportException &tte)
        {
            cerr << "ERROR: " << tte.what() << endl;
            m_is_connected = false;
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool HbCli::tableExists(const string &table)
{
    if (!isconnect())
    {
        return false;
    }
    StrVec tables;
    try
    {
        m_client.getTableNames(tables);
        for (StrVec::const_iterator it = tables.begin(); it != tables.end(); ++it)
        {
            if (table == *it)
                return true;
        }
    }
    catch (const TTransportException &tte)
    {
        cerr << "ERROR: " << tte.what() << endl;
        m_is_connected = false;
    }
    return false;
}

bool HbCli::putRow(const string &table, const string &row, const string &column, const string &value)
{
    if (!isconnect())
    {
        return false;
    }
    vector<Mutation> mutations;
    mutations.clear();
    mutations.push_back(Mutation());
    mutations.back().column = column;
    mutations.back().value = value;
    try
    {
        m_client.mutateRow(table, row, mutations, m_attributes);
        return true;
    }
    catch (const TTransportException &tte)
    {
        cerr << "ERROR: " << tte.what() << endl;
        m_is_connected = false;
        return false;
    }
}

bool HbCli::putRowWithColumns(const string &table, const string &row, const StrMap &columns)
{
    if (!isconnect())
    {
        return false;
    }
    vector<Mutation> mutations;
    mutations.clear();
    for (StrMap::const_iterator it = columns.begin(); it != columns.end(); ++it)
    {
        mutations.push_back(Mutation());
        mutations.back().column = it->first;
        mutations.back().value = it->second;
    }
    try
    {
        m_client.mutateRow(table, row, mutations, m_attributes);
        return true;
    }
    catch (const TTransportException &tte)
    {
        cerr << "ERROR: " << tte.what() << endl;
        m_is_connected = false;
        return false;
    }
}

bool HbCli::putRows(const string &table, const RowMap &rows)
{
    if (!isconnect())
    {
        return false;
    }
    guint64 start = GetCurrentMicroTime();
    vector<BatchMutation> rowBatches;
    vector<Mutation> mutations;
    for (RowMap::const_iterator it = rows.begin(); it != rows.end(); ++it)
    {
        rowBatches.push_back(BatchMutation());
        string row = it->first;
        StrMap columns = it->second;
        mutations.clear();
        for (StrMap::const_iterator iter = columns.begin(); iter != columns.end(); ++iter)
        {
            mutations.push_back(Mutation());
            mutations.back().column = iter->first;
            mutations.back().value = iter->second;
        }
        rowBatches.back().row = row;
        rowBatches.back().mutations = mutations;
    }
    try
    {
        cerr << "putRows RowMap assign vector<BatchMutation>, Cost:" << GetCurrentMicroTime() - start << "us." << endl;
        m_client.mutateRows(table, rowBatches, m_attributes);
        return true;
    }
    catch (const TTransportException &tte)
    {
        cerr << "ERROR: " << tte.what() << endl;
        m_is_connected = false;
        return false;
    }
}

bool HbCli::putRows(const string &table, const vector<BatchMutation> &rowBatches)
{
    if (!isconnect())
    {
        return false;
    }
    try
    {
        m_client.mutateRows(table, rowBatches, m_attributes);
        return true;
    }
    catch (const TTransportException &tte)
    {
        cerr << "ERROR: " << tte.what() << endl;
        m_is_connected = false;
        return false;
    }
}

bool HbCli::getRow(const string &table, const string &row, ResVec &rowResult)
{
    if (!isconnect())
    {
        return false;
    }
    try
    {
        m_client.getRow(rowResult, table, row, m_attributes);
        return true;
    }
    catch (const TTransportException &tte)
    {
        cerr << "ERROR: " << tte.what() << endl;
        m_is_connected = false;
        return false;
    }
}

bool HbCli::getRowWithColumns(const string &table, const string &row, const StrVec &columns, ResVec &rowResult)
{
    if (!isconnect())
    {
        return false;
    }
    try
    {
        m_client.getRowWithColumns(rowResult, table, row, columns, m_attributes);
        return true;
    }
    catch (const TTransportException &tte)
    {
        cerr << "ERROR: " << tte.what() << endl;
        m_is_connected = false;
        return false;
    }
}

bool HbCli::getRows(const string &table, const StrVec &rows, ResVec &rowResult)
{
    if (!isconnect())
    {
        return false;
    }
    try
    {
        m_client.getRows(rowResult, table, rows, m_attributes);
        return true;
    }
    catch (const TTransportException &tte)
    {
        cerr << "ERROR: " << tte.what() << endl;
        m_is_connected = false;
        return false;
    }
}

bool HbCli::getRowsWithColumns(const string &table, const StrVec &rows, const StrVec &columns, ResVec &rowResult)
{
    if (!isconnect())
    {
        return false;
    }
    try
    {
        m_client.getRowsWithColumns(rowResult, table, rows, columns, m_attributes);
        return true;
    }
    catch (const TTransportException &tte)
    {
        cerr << "ERROR: " << tte.what() << endl;
        m_is_connected = false;
        return false;
    }
}

bool HbCli::delRow(const string &table, const string &row)
{
    if (!isconnect())
    {
        return false;
    }
    try
    {
        m_client.deleteAllRow(table, row, m_attributes);
        return true;
    }
    catch (const TTransportException &tte)
    {
        cerr << "ERROR: " << tte.what() << endl;
        m_is_connected = false;
        return false;
    }
}

bool HbCli::delRowWithColumn(const string &table, const string &row, const string &column)
{
    if (!isconnect())
    {
        return false;
    }
    vector<Mutation> mutations;
    mutations.clear();
    mutations.push_back(Mutation());
    mutations.back().column = column;
    mutations.back().isDelete = true;
    try
    {
        m_client.mutateRow(table, row, mutations, m_attributes);
        return true;
    }
    catch (const TTransportException &tte)
    {
        cerr << "ERROR: " << tte.what() << endl;
        m_is_connected = false;
        return false;
    }
}

bool HbCli::delRowWithColumns(const string &table, const string &row, const StrVec &columns)
{
    if (!isconnect())
    {
        return false;
    }
    vector<Mutation> mutations;
    mutations.clear();
    for (StrVec::const_iterator it = columns.begin(); it != columns.end(); ++it)
    {
        mutations.push_back(Mutation());
        mutations.back().column = *it;
        mutations.back().isDelete = true;
    }
    try
    {
        m_client.mutateRow(table, row, mutations, m_attributes);
        return true;
    }
    catch (const TTransportException &tte)
    {
        cerr << "ERROR: " << tte.what() << endl;
        m_is_connected = false;
        return false;
    }
}

bool HbCli::scan(const string &table, const string &startRow, StrVec &columns, ResVec &values)
{
    if (!isconnect())
    {
        return false;
    }
    const int32_t nRows = 50;
    int scanner = m_client.scannerOpen(table, startRow, columns, m_attributes);
    try
    {
        while (true)
        {
            vector<TRowResult> value;
            m_client.scannerGetList(value, scanner, nRows);
            if (value.size() == 0)
                break;
            for (ResVec::const_iterator it = value.begin(); it != value.end(); ++it)
                values.push_back(*it);
        }
        m_client.scannerClose(scanner);
    }
    catch (const IOError &ioe)
    {
        cerr << "FATAL: Scanner raised IOError" << endl;
        m_client.scannerClose(scanner);
        return false;
    }
    catch (const TTransportException &tte)
    {
        cerr << "ERROR: " << tte.what() << endl;
        m_is_connected = false;
        m_client.scannerClose(scanner);
        return false;
    }
    return true;
}

bool HbCli::scanWithStop(const string &table, const string &startRow, const string &stopRow, StrVec &columns, ResVec &values)
{
    if (!isconnect())
    {
        return false;
    }
    const int32_t nRows = 50;
    int scanner = m_client.scannerOpenWithStop(table, startRow, stopRow, columns, m_attributes);
    try
    {
        while (true)
        {
            vector<TRowResult> value;
            m_client.scannerGetList(value, scanner, nRows);
            if (value.size() == 0)
                break;
            for (ResVec::const_iterator it = value.begin(); it != value.end(); ++it)
                values.push_back(*it);
        }
        m_client.scannerClose(scanner);
    }
    catch (const IOError &ioe)
    {
        cerr << "FATAL: Scanner raised IOError" << endl;
        m_client.scannerClose(scanner);
        return false;
    }
    catch (const TTransportException &tte)
    {
        cerr << "ERROR: " << tte.what() << endl;
        m_is_connected = false;
        m_client.scannerClose(scanner);
        return false;
    }
}

bool HbCli::getTableRegions(std::vector<TRegionInfo> &_return, const Text &tableName)
{
    if (!isconnect())
    {
        return false;
    }
    try
    {
        m_client.getTableRegions(_return, tableName);
    }
    catch (const TTransportException &tte)
    {
        cerr << "ERROR: " << tte.what() << endl;
        m_is_connected = false;
        return false;
    }
}

void HbCli::printRow(const ResVec &rowResult)
{
    for (size_t i = 0; i < rowResult.size(); i++)
    {
        cout << "row: " << rowResult[i].row << ", cols: ";
        for (CellMap::const_iterator it = rowResult[i].columns.begin();
             it != rowResult[i].columns.end(); ++it)
        {
            cout << it->first << " => " << it->second.value << "; ";
        }
        cout << endl;
    }
}
