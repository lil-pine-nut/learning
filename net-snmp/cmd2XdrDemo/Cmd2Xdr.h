#ifndef _H_Cmd2Xdr_h_
#define _H_Cmd2Xdr_h_

#include <vector>
#include <string>
using namespace std;
enum CMDTYPE
{
	CPU_CMD,
	MEM_CMD,
	NET_CMD,
	STO_CMD,
	OTHER_CMD,
};

typedef pair<int, string> CmdHostPair;

class Cmd2Xdr
{

public:
	Cmd2Xdr(){};
	virtual ~Cmd2Xdr(){};

	virtual bool init(string cmd_str, char p)
	{
		vector<string> split_vec = split_string(cmd_str, p);
		for (size_t i = 0; i < split_vec.size(); i++)
		{
			vector<string> split_vec2 = split_string(split_vec[i], ' ');
			if (split_vec2.size() < 2)
				continue;
			if (split_vec2[0].find("cpu") != string::npos)
			{
				CmdHostPair chp(CPU_CMD, split_vec2[1]);
				m_cmds.push_back(chp);
			}
			else if (split_vec2[0].find("mem") != string::npos)
			{
				CmdHostPair chp(MEM_CMD, split_vec2[1]);
				m_cmds.push_back(chp);
			}
			else if (split_vec2[0].find("net") != string::npos)
			{
				CmdHostPair chp(NET_CMD, split_vec2[1]);
				m_cmds.push_back(chp);
			}
			else if (split_vec2[0].find("store") != string::npos)
			{
				CmdHostPair chp(STO_CMD, split_vec2[1]);
				m_cmds.push_back(chp);
			}
		}
		m_separator = p;
		return (!m_cmds.empty());
	};

	virtual bool GetCpuCmd(const string &host, string &result) = 0;
	virtual bool GetMemCmd(const string &host, string &result) = 0;
	virtual bool GetNetCmd(const string &host, string &result) = 0;
	virtual bool GetStoCmd(const string &host, string &result) = 0;

	/**
	 * @brief 执行cmd
	 *
	 * @param result 返回结果
	 * @return true
	 * @return false
	 */
	virtual bool ExecuteCmd(string &result)
	{
		string cmd_result;
		bool ret = true;
		for (size_t i = 0; i < m_cmds.size();)
		{
			cmd_result.clear();
			switch (m_cmds[i].first)
			{
			case CPU_CMD:
				if (!GetCpuCmd(m_cmds[i].second, cmd_result))
				{
					ret = false;
					break;
				}
				break;

			case MEM_CMD:
				if (!GetMemCmd(m_cmds[i].second, cmd_result))
				{
					ret = false;
					break;
				}
				break;

			case NET_CMD:
				if (!GetNetCmd(m_cmds[i].second, cmd_result))
				{
					ret = false;
					break;
				}
				break;

			case STO_CMD:
				if (!GetStoCmd(m_cmds[i].second, cmd_result))
				{
					ret = false;
					break;
				}
				break;

			default:
				break;
			}

			result += cmd_result;
			if (++i < m_cmds.size())
			{
				result += m_separator;
			}
		}
		if (!ret)
		{
			result.clear();
			return false;
		}
		return true;
	};

	virtual void TimeLog(){};

public:
	vector<CmdHostPair> m_cmds;
	char m_separator;
};

#endif
