#include "Snmp2Xdr.h"
#include "Log.h"
#include <fstream>

Snmp2Xdr::Snmp2Xdr()
{
    char buff[128] = {0};
    sprintf(buff, "%d", getpid());
    m_pid_str = buff;
}

Snmp2Xdr::~Snmp2Xdr()
{
}

bool CallSystem(const string &cmd)
{
    int status = system(cmd.c_str());
    if (status < 0)
    {
        ostringstream oss;
        oss << "cmd: " << cmd.c_str() << " \t error:" << strerror(errno) << endl;
        LOG_ERROR(oss.str().c_str());
        return false;
    }

    if (WIFEXITED(status))
    {
        return true;
    }
    // else if(WIFSIGNALED(status))
    // {
    //     ostringstream oss;
    //     oss << "abnormal termination, signal number = "<< WTERMSIG(status) << endl; //如果cmdstring被信号中断，取得信号值
    //     LOG_ERROR(oss.str().c_str());
    // }
    // else if(WIFSTOPPED(status))
    // {
    //     ostringstream oss;
    //     oss << "abnormal stopped, signal number = "<< WSTOPSIG(status) << endl; //如果cmdstring被信号暂停执行，取得信号值
    //     LOG_ERROR(oss.str().c_str());
    // }
    return false;
}

string GetSnmpgetValidStr(const string &file)
{
    ifstream ifile;
    string ret_str;
    ifile.open(file.c_str());
    if (!ifile.is_open())
    {
        remove(file.c_str());
        return ret_str;
    }
    string line;
    getline(ifile, line);
    if (line.empty())
        return ret_str;
    vector<string> split_vec = split_string(line, ' ');
    if (split_vec.size() < 4)
        return ret_str;
    if (split_vec[0].find("MIB") == string::npos && split_vec[1] != "=")
    {
        ostringstream oss;
        oss << "snmpget: " << line << endl;
        LOG_ERROR(oss.str().c_str());
        ifile.close();
        remove(file.c_str());
        return ret_str;
    }
    ifile.close();
    remove(file.c_str());
    ret_str = split_vec[3];
    return ret_str;
}

bool Snmp2Xdr::GetCpuCmd(const string &host, string &result)
{
    string get_cpu_file = "/tmp/cmd2xdr_getcpu_" + m_pid_str + ".txt";
    // ssCpuIdle, 空闲CPU百分比
    string cmd_str = "snmpget -v 2c -c public " + host + " .1.3.6.1.4.1.2021.11.11.0 > " + get_cpu_file + " 2>&1";
    if (!CallSystem(cmd_str))
    {
        remove(get_cpu_file.c_str());
        return false;
    }
    string cpu_use_str = GetSnmpgetValidStr(get_cpu_file);
    if (cpu_use_str.empty())
        return false;
    result = itoa((double)100 - atof(cpu_use_str.c_str()));
    cerr << result << endl;
    return true;
}

bool Snmp2Xdr::GetMemCmd(const string &host, string &result)
{
    string get_mem_file = "/tmp/cmd2xdr_get_mem_" + m_pid_str + ".txt";
    // memTotalReal, Total RAM in machine
    string cmd_str = "snmpget -v 2c -c public " + host + " .1.3.6.1.4.1.2021.4.5.0 > " + get_mem_file + " 2>&1";
    if (!CallSystem(cmd_str))
    {
        remove(get_mem_file.c_str());
        return false;
    }
    string mem_total_str = GetSnmpgetValidStr(get_mem_file);
    if (mem_total_str.empty())
        return false;

    // memAvailReal, Total RAM Avail used
    cmd_str = "snmpget -v 2c -c public " + host + " .1.3.6.1.4.1.2021.4.6.0 > " + get_mem_file + " 2>&1";
    if (!CallSystem(cmd_str))
    {
        remove(get_mem_file.c_str());
        return false;
    }
    string mem_avail_str = GetSnmpgetValidStr(get_mem_file);
    if (mem_avail_str.empty())
        return false;

    // memBuffer, Total RAM Buffered
    cmd_str = "snmpget -v 2c -c public " + host + " .1.3.6.1.4.1.2021.4.14.0 > " + get_mem_file + " 2>&1";
    if (!CallSystem(cmd_str))
    {
        remove(get_mem_file.c_str());
        return false;
    }
    string mem_buff_str = GetSnmpgetValidStr(get_mem_file);
    if (mem_buff_str.empty())
        return false;

    // memCached, Total Cached Memory
    cmd_str = "snmpget -v 2c -c public " + host + " .1.3.6.1.4.1.2021.4.15.0 > " + get_mem_file + " 2>&1";
    if (!CallSystem(cmd_str))
    {
        remove(get_mem_file.c_str());
        return false;
    }
    string mem_cache_str = GetSnmpgetValidStr(get_mem_file);
    if (mem_cache_str.empty())
        return false;
    // cerr << mem_cache_str << endl;

    result = lltoa(atoll(mem_total_str.c_str()) - atoll(mem_avail_str.c_str()) -
                   atoll(mem_buff_str.c_str()) - atoll(mem_cache_str.c_str()));
    cerr << result << endl;
    return true;
}

vector<string> GetSnmpwalkValidStr(const string &file)
{
    ifstream ifile;
    vector<string> ret_vec;
    ifile.open(file.c_str());
    if (!ifile.is_open())
    {
        remove(file.c_str());
        return ret_vec;
    }

    string line;
    while (getline(ifile, line))
    {
        if (line.empty())
            continue;
        vector<string> split_vec = split_string(line, ' ');
        if (split_vec.size() < 4)
            continue;
        if (split_vec[0].find("MIB") == string::npos && split_vec[1] != "=")
        {
            ostringstream oss;
            oss << "snmpget: " << line << endl;
            LOG_ERROR(oss.str().c_str());
            ifile.close();
            remove(file.c_str());
            ret_vec.clear();
            return ret_vec;
        }
        ret_vec.push_back(split_vec[3]);
    }
    ifile.close();
    remove(file.c_str());

    return ret_vec;
}

bool Snmp2Xdr::GetNetCmd(const string &host, string &result)
{
    string get_net_file = "/tmp/cmd2xdr_getnet_" + m_pid_str + ".txt";
    // ifDescr, 网络接口信息描述
    string cmd_str = "snmpwalk -v 2c -c public " + host + " .1.3.6.1.2.1.2.2.1.2 > " + get_net_file + " 2>&1";
    if (!CallSystem(cmd_str))
    {
        remove(get_net_file.c_str());
        return false;
    }
    vector<string> if_des_vec = GetSnmpwalkValidStr(get_net_file);
    if (if_des_vec.empty())
        return false;

    // ifOperStatus, 接口当前操作状态[up or down]
    cmd_str = "snmpwalk -v 2c -c public " + host + " .1.3.6.1.2.1.2.2.1.8 > " + get_net_file + " 2>&1";
    if (!CallSystem(cmd_str))
    {
        remove(get_net_file.c_str());
        return false;
    }
    vector<string> if_status_vec = GetSnmpwalkValidStr(get_net_file);
    if (if_status_vec.empty())
        return false;

    // ifInOctets, 接口收到的字节数
    cmd_str = "snmpwalk -v 2c -c public " + host + " .1.3.6.1.2.1.2.2.1.10 > " + get_net_file + " 2>&1";
    if (!CallSystem(cmd_str))
    {
        remove(get_net_file.c_str());
        return false;
    }
    vector<string> if_recv_vec = GetSnmpwalkValidStr(get_net_file);
    if (if_recv_vec.empty())
        return false;

    // ifOutOctets, 接口发送的字节数
    cmd_str = "snmpwalk -v 2c -c public " + host + " .1.3.6.1.2.1.2.2.1.16 > " + get_net_file + " 2>&1";
    if (!CallSystem(cmd_str))
    {
        remove(get_net_file.c_str());
        return false;
    }
    vector<string> if_send_vec = GetSnmpwalkValidStr(get_net_file);
    if (if_send_vec.empty())
        return false;

    if (if_des_vec.size() != if_status_vec.size() && if_des_vec.size() != if_recv_vec.size() && if_des_vec.size() != if_send_vec.size())
        return false;

    int if_num = 0;
    for (size_t i = 0; i < if_des_vec.size(); i++)
    {
        // cerr << "name:" << if_des_vec[i] << "\tis_up:" << if_status_vec[i] << "\tinOctet:" << if_recv_vec[i] << "\toutOctet:" << if_send_vec[i] << endl;
        if (strncmp(if_status_vec[i].c_str(), "up", 2) != 0)
            continue;
        result += if_des_vec[i] + '|' + if_recv_vec[i] + '|' + if_send_vec[i];
        if (++if_num < 10)
            result += '|';
        else
            break;
    }
    for (int i = if_num; i < 10;)
    {
        ++i;
        result += "NULL|NULL|NULL";
        if (i < 10)
            result += '|';
    }
    cerr << result << endl;
    return true;
}

bool Snmp2Xdr::GetStoCmd(const string &host, string &result)
{
    string get_sto_file = "/tmp/cmd2xdr_getsto_" + m_pid_str + ".txt";
    // hrStorageDescr, 存储设备描述
    string cmd_str = "snmpwalk -v 2c -c public " + host + " .1.3.6.1.2.1.25.2.3.1.3 > " + get_sto_file + " 2>&1";
    if (!CallSystem(cmd_str))
    {
        remove(get_sto_file.c_str());
        return false;
    }
    vector<string> sto_des_vec = GetSnmpwalkValidStr(get_sto_file);
    if (sto_des_vec.empty())
        return false;

    // hrStorageSize, 簇的的数目
    cmd_str = "snmpwalk -v 2c -c public " + host + " .1.3.6.1.2.1.25.2.3.1.5 > " + get_sto_file + " 2>&1";
    if (!CallSystem(cmd_str))
    {
        remove(get_sto_file.c_str());
        return false;
    }
    vector<string> sto_size_vec = GetSnmpwalkValidStr(get_sto_file);
    if (sto_size_vec.empty())
        return false;

    // hrStorageUsed, 簇的使用数目
    cmd_str = "snmpwalk -v 2c -c public " + host + " .1.3.6.1.2.1.25.2.3.1.6 > " + get_sto_file + " 2>&1";
    if (!CallSystem(cmd_str))
    {
        remove(get_sto_file.c_str());
        return false;
    }
    vector<string> sto_use_vec = GetSnmpwalkValidStr(get_sto_file);
    if (sto_use_vec.empty())
        return false;

    if (sto_des_vec.size() != sto_size_vec.size() && sto_des_vec.size() != sto_use_vec.size())
        return false;

    int sto_num = 0;
    for (size_t i = 0; i < sto_des_vec.size(); i++)
    {
        // cerr << "name:" << sto_des_vec[i] << "\t" << "size:" << sto_size_vec[i] << "\t" << "use:" << sto_use_vec[i] << endl;
        if (sto_des_vec[i].find('/') == string::npos) // 跳过前面不带'/'的Storage
            continue;

        result += sto_des_vec[i] + '|' + ftoa(atof(sto_use_vec[i].c_str()) / atof(sto_size_vec[i].c_str()) * 100);
        if (++sto_num < 10)
            result += '|';
        else
            break;
    }
    for (int i = sto_num; i < 10;)
    {
        ++i;
        result += "NULL|NULL";
        if (i < 10)
            result += '|';
    }
    cerr << result << endl;
    return true;
}

void Snmp2Xdr::TimeLog()
{
}