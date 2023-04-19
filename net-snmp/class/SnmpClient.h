#ifndef __SNMPCLIENT__H__
#define __SNMPCLIENT__H__

#include <iostream>
#include <vector>
using namespace std;

class SnmpClient
{

public:
    SnmpClient(/* args */);
    ~SnmpClient();

    bool Init(const string &host, const string &community);

    bool SnmpGet(const char *oid_names, string &ret_string);

    bool SnmpWalk(const char *oid_names, vector<string> &ret_strs);

private:
    void *m_session, *m_session_open;
};

#endif