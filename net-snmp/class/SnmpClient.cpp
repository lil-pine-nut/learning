#include "SnmpClient.h"
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#ifdef HAVE_WINSOCK_H
#include <winsock.h>
#endif

SnmpClient::SnmpClient(/* args */)
{
    m_session = new snmp_session();
    m_session_open = NULL;
}

SnmpClient::~SnmpClient()
{
    if (m_session_open)
        snmp_close((snmp_session *)m_session_open);
    delete m_session;
}

void LogSnmppError(const char *msg, snmp_session *ss)
{
    char *err_buf = NULL;
    int clib_errorno = 0, snmp_errorno = 0;
    snmp_error(ss, &clib_errorno, &snmp_errorno, &err_buf);

    fprintf(stderr, "%s: %s.\n", msg, err_buf);
    if (err_buf)
        free(err_buf);
}

bool SnmpClient::Init(const string &host, const string &community)
{
    struct snmp_session *sp;
    snmp_session *ss = (snmp_session *)m_session;
    snmp_sess_init(ss); /* initialize session */
    ss->version = SNMP_VERSION_2c;
    ss->peername = strdup(host.c_str());
    ss->community = (u_char *)strdup(community.c_str());
    ss->community_len = strlen((char *)ss->community);
    if (!(sp = snmp_open(ss)))
    {
        // snmp_perror("snmp_open");
        LogSnmppError("snmp_open", ss);
        return false;
    }
    m_session_open = sp;
    return true;
}

bool SnmpClient::SnmpGet(const char *oid_names, string &ret_string)
{

    struct snmp_pdu *pdu, *response;
    oid name[MAX_OID_LEN];
    size_t name_length;

    pdu = snmp_pdu_create(SNMP_MSG_GET);
    name_length = MAX_OID_LEN;
    if (!snmp_parse_oid(oid_names, name, &name_length))
    {
        snmp_perror(oid_names);
        return false;
    }
    else
        snmp_add_null_var(pdu, name, name_length);

    bool ret = false;
    struct snmp_session *sp = (snmp_session *)m_session_open;
    struct snmp_session *ss = (snmp_session *)m_session;
    int status = snmp_synch_response(sp, pdu, &response);
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
        fprintf(stderr, "Timeout: No Response from %s\n", ss->peername);
    }
    else
    {
        // snmp_sess_perror("snmpget", ss);
        LogSnmppError("snmpget", ss);
    }
    if (response)
        snmp_free_pdu(response);

    return ret;
}

bool SnmpClient::SnmpWalk(const char *oid_names, vector<string> &ret_strs)
{
    oid name[MAX_OID_LEN];
    size_t name_length;
    name_length = MAX_OID_LEN;
    if (snmp_parse_oid(oid_names, name, &name_length) == NULL)
    {
        snmp_perror(oid_names);
        return false;
    }

    struct snmp_pdu *pdu, *response;
    oid end_oid[MAX_OID_LEN];
    size_t end_len = 0;
    netsnmp_variable_list *vars;
    struct snmp_session *sp = (snmp_session *)m_session_open;
    struct snmp_session *ss = (snmp_session *)m_session;
    int running = 1, status = STAT_ERROR;
    string get_str;

    memmove(end_oid, name, name_length * sizeof(oid));
    end_len = name_length;
    end_oid[end_len - 1]++;
    char buf[4096];
    while (running)
    {
        get_str.clear();
        pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);
        snmp_add_null_var(pdu, name, name_length);
        status = snmp_synch_response(sp, pdu, &response);
        if (status == STAT_SUCCESS)
        {
            if (response->errstat == SNMP_ERR_NOERROR)
            {
                /*
                 * check resulting variables
                 */
                for (vars = response->variables; vars; vars = vars->next_variable)
                {
                    if (snmp_oid_compare(end_oid, end_len, vars->name, vars->name_length) <= 0)
                    {
                        /*
                         * not part of this subtree
                         */
                        running = 0;
                        continue;
                    }
                    snprint_variable(buf, sizeof(buf), vars->name, vars->name_length, vars);
                    get_str += buf;
                    // print_variable(vars->name, vars->name_length, vars);

                    if ((vars->type != SNMP_ENDOFMIBVIEW) &&
                        (vars->type != SNMP_NOSUCHOBJECT) &&
                        (vars->type != SNMP_NOSUCHINSTANCE))
                    {
                        memmove((char *)name, (char *)vars->name,
                                vars->name_length * sizeof(oid));
                        name_length = vars->name_length;
                    }
                    else
                        running = 0;
                }
                if (!get_str.empty())
                    ret_strs.push_back(get_str);
            }
            else
            {
                /*
                 * error in resp, print it
                 */
                running = 0;
                if (response->errstat == SNMP_ERR_NOSUCHNAME)
                {
                    printf("End of MIB\n");
                }
                else
                {
                    fprintf(stderr, "Error in packet.\nReason: %s\n",
                            snmp_errstring(response->errstat));
                    if (response->errindex != 0)
                    {
                        fprintf(stderr, "Failed object: ");
                        if (vars)
                            fprint_objid(stderr, vars->name,
                                         vars->name_length);
                        fprintf(stderr, "\n");
                    }
                }
            }
        }
        else if (status == STAT_TIMEOUT)
        {
            fprintf(stderr, "Timeout: No Response from %s\n",
                    ss->peername);
            running = 0;
        }
        else
        { /* status == STAT_ERROR */
            // snmp_sess_perror("snmpwalk", ss);
            LogSnmppError("snmpwalk", ss);
            running = 0;
        }
        if (response)
            snmp_free_pdu(response);
    }
    return !ret_strs.empty();
}