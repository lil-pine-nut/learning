#include "SnmpManager.h"
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#ifdef HAVE_WINSOCK_H
#include <winsock.h>
#endif

SnmpManager::SnmpManager(/* args */)
{
    /* Win32: init winsock */
    SOCK_STARTUP;

    /* initialize library */
    init_snmp("SnmpManager");
}

SnmpManager::~SnmpManager()
{
    SnmpClientMap::iterator it = m_SnmpClientMap.begin();
    while (it != m_SnmpClientMap.end())
    {
        delete it->second;
        m_SnmpClientMap.erase(it++);
    }
}

bool SnmpManager::CreateClient(const string &host, const string &community)
{
    if (m_SnmpClientMap.find(host) != m_SnmpClientMap.end())
        return false;

    SnmpClient *pSnmpClient = new SnmpClient();
    if (NULL == pSnmpClient)
    {
        cerr << "new SnmpClient Failed!" << endl;
        return false;
    }
    if (!pSnmpClient->Init(host, community))
    {
        delete pSnmpClient;
        return false;
    }
    m_SnmpClientMap.insert(SnmpClientMap::value_type(host, pSnmpClient));
    return true;
}

void split_string(const string &str, char p, std::vector<string> *vec)
{
    string::size_type pos1, pos2;
    pos2 = str.find(p);
    pos1 = 0;

    while (string::npos != pos2)
    {
        if (pos2 - pos1 > 0)
        {
            vec->push_back(str.substr(pos1, pos2 - pos1));
        }

        pos1 = pos2 + 1;
        pos2 = str.find(p, pos1);
    }
    if (str.size() - pos1 > 0)
    {
        vec->push_back(str.substr(pos1));
    }
}

double SnmpManager::GetCpuUsage(const string &host)
{
    SnmpClientMap::iterator it = m_SnmpClientMap.find(host);
    if (it == m_SnmpClientMap.end())
        return -1.0;
    string ret_string;
    if (it->second->SnmpGet(".1.3.6.1.4.1.2021.11.11.0", ret_string))
    {
        // cerr << ret_string << endl;
        vector<string> split_vec;
        split_string(ret_string, ' ', &split_vec);
        if (split_vec.size() >= 4)
        {
            // cerr << "split_vec[3]:" << split_vec[3] << endl;
            return ((double)100 - atof(split_vec[3].c_str()));
        }
    }
    return -1.0;
}

long long SnmpManager::GetMemUseSize(const string &host)
{
    // 根据 free 命令可知: used = total - free - (buff/cache)
    SnmpClientMap::iterator it = m_SnmpClientMap.find(host);
    if (it == m_SnmpClientMap.end())
        return -1;

    string ret_string;
    long long mem_total_size = 0, mem_avail_size = 0, mem_buff_size = 0, mem_cache_size = 0;
    if (it->second->SnmpGet(".1.3.6.1.4.1.2021.4.5.0", ret_string))
    {
        // cerr << ret_string << endl;
        vector<string> split_vec;
        split_string(ret_string, ' ', &split_vec);
        if (split_vec.size() >= 4)
        {
            // cerr << "split_vec[3]:" << split_vec[3] << endl;
            mem_total_size = atoll(split_vec[3].c_str());
        }
    }
    else
        return -1;

    ret_string.clear();
    if (it->second->SnmpGet(".1.3.6.1.4.1.2021.4.6.0", ret_string))
    {
        // cerr << ret_string << endl;
        vector<string> split_vec;
        split_string(ret_string, ' ', &split_vec);
        if (split_vec.size() >= 4)
        {
            // cerr << "split_vec[3]:" << split_vec[3] << endl;
            mem_avail_size = atoll(split_vec[3].c_str());
        }
    }
    else
        return -1;

    ret_string.clear();
    if (it->second->SnmpGet(".1.3.6.1.4.1.2021.4.14.0", ret_string))
    {
        // cerr << ret_string << endl;
        vector<string> split_vec;
        split_string(ret_string, ' ', &split_vec);
        if (split_vec.size() >= 4)
        {
            // cerr << "split_vec[3]:" << split_vec[3] << endl;
            mem_buff_size = atoll(split_vec[3].c_str());
        }
    }
    else
        return -1;

    ret_string.clear();
    if (it->second->SnmpGet(".1.3.6.1.4.1.2021.4.15.0", ret_string))
    {
        // cerr << ret_string << endl;
        vector<string> split_vec;
        split_string(ret_string, ' ', &split_vec);
        if (split_vec.size() >= 4)
        {
            // cerr << "split_vec[3]:" << split_vec[3] << endl;
            mem_cache_size = atoll(split_vec[3].c_str());
        }
    }
    else
        return -1;

    return (mem_total_size - mem_avail_size - mem_buff_size - mem_cache_size);
}

bool SnmpManager::GetHardStorageUsage(const string &host, vector<HardStorage> &HardStorages)
{
    SnmpClientMap::iterator it = m_SnmpClientMap.find(host);
    if (it == m_SnmpClientMap.end())
        return false;

    vector<string> storage_descr_vec, storage_size_vec, storage_use_vec;
    if (!(it->second->SnmpWalk(".1.3.6.1.2.1.25.2.3.1.3", storage_descr_vec)))
        return false;
    if (!(it->second->SnmpWalk(".1.3.6.1.2.1.25.2.3.1.5", storage_size_vec)))
        return false;
    if (!(it->second->SnmpWalk(".1.3.6.1.2.1.25.2.3.1.6", storage_use_vec)))
        return false;
    if (storage_descr_vec.empty() || storage_size_vec.empty() || storage_use_vec.empty())
        return false;
    if (storage_descr_vec.size() != storage_size_vec.size() && storage_descr_vec.size() != storage_use_vec.size())
        return false;

    for (size_t i = 0; i < storage_descr_vec.size(); i++)
    {
        if (storage_descr_vec[i].find('/') == string::npos) // 跳过前面不带'/'的Storage
            continue;
        vector<string> split_vec1, split_vec2, split_vec3;
        split_string(storage_descr_vec[i], ' ', &split_vec1);
        split_string(storage_size_vec[i], ' ', &split_vec2);
        split_string(storage_use_vec[i], ' ', &split_vec3);
        if (split_vec1.size() < 4 || split_vec2.size() < 4 || split_vec3.size() < 4)
            continue;
        // cerr << "name:" << split_vec1[3] << "\t" << "size:" << split_vec2[3]
        // << "\t" << "use:" << split_vec3[3] << endl;

        HardStorage hrStorage;
        hrStorage.name = split_vec1[3];
        hrStorage.usage = (double)atoll(split_vec3[3].c_str()) / atoll(split_vec2[3].c_str()) * 100;
        HardStorages.push_back(hrStorage);
    }
    return (!HardStorages.empty());
}

bool SnmpManager::GetEthIfInfo(const string &host, vector<EthIf> &EthIfs)
{
    SnmpClientMap::iterator it = m_SnmpClientMap.find(host);
    if (it == m_SnmpClientMap.end())
        return false;

    vector<string> if_descr_vec, if_status_vec, if_speed_vec, if_inOctet_vec, if_outOctet_vec;
    if (!(it->second->SnmpWalk(".1.3.6.1.2.1.2.2.1.2", if_descr_vec)))
        return false;
    if (!(it->second->SnmpWalk(".1.3.6.1.2.1.2.2.1.8", if_status_vec)))
        return false;
    if (!(it->second->SnmpWalk(".1.3.6.1.2.1.2.2.1.5", if_speed_vec)))
        return false;
    if (!(it->second->SnmpWalk(".1.3.6.1.2.1.2.2.1.10", if_inOctet_vec)))
        return false;
    if (!(it->second->SnmpWalk(".1.3.6.1.2.1.2.2.1.16", if_outOctet_vec)))
        return false;
    if (if_descr_vec.empty() || if_status_vec.empty() || if_speed_vec.empty() || if_inOctet_vec.empty() || if_outOctet_vec.empty())
        return false;
    if (if_descr_vec.size() != if_status_vec.size() && if_descr_vec.size() != if_speed_vec.size() && if_descr_vec.size() != if_inOctet_vec.size() && if_descr_vec.size() != if_outOctet_vec.size())
        return false;

    for (size_t i = 0; i < if_descr_vec.size(); i++)
    {
        vector<string> split_vec1, split_vec2, split_vec3, split_vec4, split_vec5;
        split_string(if_descr_vec[i], ' ', &split_vec1);
        split_string(if_status_vec[i], ' ', &split_vec2);
        split_string(if_speed_vec[i], ' ', &split_vec3);
        split_string(if_inOctet_vec[i], ' ', &split_vec4);
        split_string(if_outOctet_vec[i], ' ', &split_vec5);
        if (split_vec1.size() < 4 || split_vec2.size() < 4 || split_vec3.size() < 4 || split_vec4.size() < 4 || split_vec5.size() < 4)
            continue;
        // cerr << "name:" << split_vec1[3] << "\tis_up:" << split_vec2[3] << "\tbandwidth:" << split_vec2[3]
        // << "\tinOctet:" << split_vec3[3] << "\toutOctet:" << split_vec4[3] << endl;

        EthIf ethif;
        ethif.name = split_vec1[3];
        if (strncmp(split_vec2[3].c_str(), "up", 2) == 0)
            ethif.is_up = true;
        ethif.bandwidth = atoll(split_vec3[3].c_str());
        ethif.inOctet = atoll(split_vec4[3].c_str());
        ethif.outOctet = atoll(split_vec5[3].c_str());
        EthIfs.push_back(ethif);
    }
    return (!EthIfs.empty());
}