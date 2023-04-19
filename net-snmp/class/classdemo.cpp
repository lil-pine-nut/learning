#include "SnmpManager.h"
#include <stdio.h>

int main()
{
    // for (size_t i = 0; i < 1000; i++)
    {
        SnmpManager *snmpManager = new SnmpManager();
        if (snmpManager->CreateClient("127.0.0.1", "public"))
        {
            cerr << "GetCpuUsage:" << snmpManager->GetCpuUsage("127.0.0.1") << endl;

            cerr << "GetMemUseSize:" << snmpManager->GetMemUseSize("127.0.0.1") << endl;

            vector<HardStorage> HardStorages;
            snmpManager->GetHardStorageUsage("127.0.0.1", HardStorages);
            for (size_t i = 0; i < HardStorages.size(); i++)
            {
                cerr << HardStorages[i].name << "\t" << HardStorages[i].usage << endl;
            }
            cerr << endl;
            vector<EthIf> EthIfs;
            snmpManager->GetEthIfInfo("127.0.0.1", EthIfs);
            for (size_t i = 0; i < EthIfs.size(); i++)
            {
                cerr << EthIfs[i].name << "\t" << EthIfs[i].is_up << "\t" << EthIfs[i].bandwidth
                     << "\t" << EthIfs[i].inOctet << "\t" << EthIfs[i].outOctet << endl;
            }
        }
        // getchar();
        delete snmpManager;
    }
    // getchar();
    return 0;
}