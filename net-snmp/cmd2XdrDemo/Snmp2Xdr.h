#ifndef _H_Snmp2Xdr_h_
#define _H_Snmp2Xdr_h_

#include "Cmd2Xdr.h"

using namespace std;

class Snmp2Xdr : public Cmd2Xdr
{

public:
	Snmp2Xdr();
	virtual ~Snmp2Xdr();

	virtual bool GetCpuCmd(const string &host, string &result);
	virtual bool GetMemCmd(const string &host, string &result);
	virtual bool GetNetCmd(const string &host, string &result);
	virtual bool GetStoCmd(const string &host, string &result);

	virtual void TimeLog();

private:
	string m_pid_str;
};

#endif
