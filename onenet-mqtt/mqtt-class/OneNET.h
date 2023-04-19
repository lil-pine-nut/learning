#ifndef __ONENET__H__
#define __ONENET__H__

#include <string>
using namespace std;

struct OneNET
{
    string key;
    string res;
    string et;
    string method;
    string version;

    string Token();
};

#endif