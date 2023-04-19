#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#ifdef HAVE_WINSOCK_H
#include <winsock.h>
#endif

#include <vector>
#include <iostream>
#include <unistd.h>
using namespace std;

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

struct host
{
    const char *name;
    const char *community;
};

/*
 * initialize
 */
void initialize(void)
{
    /* Win32: init winsock */
    SOCK_STARTUP;

    /* initialize library */
    init_snmp("net-snmp");
}

struct snmp_session ss, *sp;
bool InitClient(struct host *hp)
{
    snmp_sess_init(&ss); /* initialize session */
    ss.version = SNMP_VERSION_2c;
    ss.peername = strdup(hp->name);
    ss.community = (u_char *)strdup(hp->community);
    ss.community_len = strlen((char *)ss.community);
    if (!(sp = snmp_open(&ss)))
    {
        snmp_perror("snmp_open");
        return false;
    }
    return true;
}

void CloseClient()
{
    snmp_close(sp);
}

bool SnmpGet(const char *oid_names, int oid_length, string &ret_string)
{
    bool ret = false;
    struct snmp_pdu *pdu, *response;
    oid name[MAX_OID_LEN];
    size_t name_length;
    int status;
    pdu = snmp_pdu_create(SNMP_MSG_GET);

    name_length = MAX_OID_LEN;
    if (!snmp_parse_oid(oid_names, name, &name_length))
    {
        snmp_perror(oid_names);
        return false;
    }
    else
        snmp_add_null_var(pdu, name, name_length);

    status = snmp_synch_response(sp, pdu, &response);
    if (status == STAT_SUCCESS)
    {
        if (response->errstat == SNMP_ERR_NOERROR)
        {
            ret = true;
            char buf[4096];
            struct variable_list *vars;
            for (vars = response->variables; vars; vars = vars->next_variable)
            {
                snprint_variable(buf, sizeof(buf), vars->name, vars->name_length, vars);
                // fprintf(stderr, "%s\n", buf);
                ret_string += buf;
            }
        }
        else
        {
            fprintf(stderr, "Error in packet\nReason: %s\n", snmp_errstring(response->errstat));
        }
    }
    else if (status == STAT_TIMEOUT)
    {
        fprintf(stderr, "Timeout: No Response from %s\n", ss.peername);
    }
    else
    {
        snmp_sess_perror("snmpget", &ss);
    }
    if (response)
        snmp_free_pdu(response);

    return ret;
}

double GetCpuUsage()
{
    string ret_string;
    if (SnmpGet(".1.3.6.1.4.1.2021.11.11.0", 25, ret_string))
    {
        // cerr << ret_string << endl;
        vector<string> split_vec;
        split_string(ret_string, ' ', &split_vec);
        if (split_vec.size() >= 4)
        {
            // cerr << split_vec[3] << endl;
            return (100 - atof(split_vec[3].c_str()));
        }
    }
    return 0.0;
}

int main()
{
    initialize();
    struct host host_info;
    host_info.name = "127.0.0.1";
    host_info.community = "public";
    InitClient(&host_info);
    for (size_t i = 0; i < 360; i++)
    {
        cerr << "i:" << i << "\t"
             << "GetCpuUsage:" << GetCpuUsage() << endl;
        sleep(10);
    }

    return 0;
}