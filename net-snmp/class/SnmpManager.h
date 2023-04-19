#ifndef __SNMPMANAGER__H__
#define __SNMPMANAGER__H__

#include <map>
#include <vector>
#include "SnmpClient.h"

typedef map<string, SnmpClient *> SnmpClientMap;

struct HardStorage
{
    HardStorage()
    {
        usage = 0.0;
    }
    string name;
    double usage;
};

typedef struct EthernetInterface
{
    EthernetInterface()
    {
        is_up = false;
        bandwidth = 0;
        inOctet = 0;
        outOctet = 0;
    }
    string name;
    bool is_up;
    size_t bandwidth;
    size_t inOctet;
    size_t outOctet;
} EthIf;

class SnmpManager
{

public:
    SnmpManager(/* args */);
    ~SnmpManager();

    bool CreateClient(const string &host, const string &community);

public:
    double GetCpuUsage(const string &host);

    long long GetMemUseSize(const string &host);

    bool GetHardStorageUsage(const string &host, vector<HardStorage> &HardDisks);

    bool GetEthIfInfo(const string &host, vector<EthIf> &EthIfs);

private:
    SnmpClientMap m_SnmpClientMap;
};

#endif