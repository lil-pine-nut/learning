/*
 * NET-SNMP demo
 *
 * This program demonstrates different ways to query a list of hosts
 * for a list of variables.
 *
 * It would of course be faster just to send one query for all variables,
 * but the intention is to demonstrate the difference between synchronous
 * and asynchronous operation.
 *
 * Niels Baggesen (Niels.Baggesen@uni-c.dk), 1999.
 */

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#ifdef HAVE_WINSOCK_H
#include <winsock.h>
#endif

/*
 * a list of hosts to query
 */
struct host
{
    const char *name;
    const char *community;
} hosts[] = {
    {"127.0.0.1", "public"},
    {NULL}};

/*
 * a list of variables to query for
 */
struct oid
{
    const char *Name;
    oid Oid[MAX_OID_LEN];
    int OidLen;
} oids[] = {
    {".1.3.6.1.2.1.2.2.1.2"},
    // {".1.3.6.1.2.1.2.2.1.3"},
    {NULL}};

/*
 * initialize
 */
void initialize(void)
{
    struct oid *op = oids;

    /* Win32: init winsock */
    SOCK_STARTUP;

    /* initialize library */
    init_snmp("asynchapp");

    /* parse the oids */
    while (op->Name)
    {
        op->OidLen = sizeof(op->Oid) / sizeof(op->Oid[0]);
        if (!read_objid(op->Name, op->Oid, &op->OidLen))
        {
            snmp_perror("read_objid");
            exit(1);
        }
        op++;
    }
}

int running = 1;
oid name[MAX_OID_LEN];
size_t name_length;

#define NETSNMP_DS_WALK_INCLUDE_REQUESTED 1
#define NETSNMP_DS_WALK_PRINT_STATISTICS 2
#define NETSNMP_DS_WALK_DONT_CHECK_LEXICOGRAPHIC 3
#define NETSNMP_DS_WALK_TIME_RESULTS 4
#define NETSNMP_DS_WALK_DONT_GET_REQUESTED 5
#define NETSNMP_DS_WALK_TIME_RESULTS_SINGLE 6
/*****************************************************************************/

void snmp_get_and_print(netsnmp_session *ss, oid *theoid, size_t theoid_len)
{
    netsnmp_pdu *pdu, *resp;
    netsnmp_variable_list *vars;
    int status;

    pdu = snmp_pdu_create(SNMP_MSG_GET);
    snmp_add_null_var(pdu, theoid, theoid_len);

    status = snmp_synch_response(ss, pdu, &resp);
    if (status == STAT_SUCCESS && resp->errstat == SNMP_ERR_NOERROR)
    {
        for (vars = resp->variables; vars; vars = vars->next_variable)
        {
            print_variable(vars->name, vars->name_length, vars);
        }
    }
    if (resp)
    {
        snmp_free_pdu(resp);
    }
}

/*
 * simple synchronous loop
 */
void synchronous(void)
{
    struct host *hp;
    oid end_oid[MAX_OID_LEN];
    size_t end_len = 0;
    struct oid root[MAX_OID_LEN];
    size_t rootlen;
    // netsnmp_ds_register_config(ASN_BOOLEAN, "snmpwalk", "includeRequested",
    // 		       NETSNMP_DS_APPLICATION_ID,
    // 		       NETSNMP_DS_WALK_INCLUDE_REQUESTED);

    // netsnmp_ds_register_config(ASN_BOOLEAN, "snmpwalk", "excludeRequested",
    // 		       NETSNMP_DS_APPLICATION_ID,
    // 		       NETSNMP_DS_WALK_DONT_GET_REQUESTED);

    // netsnmp_ds_register_config(ASN_BOOLEAN, "snmpwalk", "printStatistics",
    // 		       NETSNMP_DS_APPLICATION_ID,
    // 		       NETSNMP_DS_WALK_PRINT_STATISTICS);

    // netsnmp_ds_register_config(ASN_BOOLEAN, "snmpwalk", "dontCheckOrdering",
    // 		       NETSNMP_DS_APPLICATION_ID,
    // 		       NETSNMP_DS_WALK_DONT_CHECK_LEXICOGRAPHIC);

    // netsnmp_ds_register_config(ASN_BOOLEAN, "snmpwalk", "timeResults",
    //                            NETSNMP_DS_APPLICATION_ID,
    //                            NETSNMP_DS_WALK_TIME_RESULTS);

    // netsnmp_ds_register_config(ASN_BOOLEAN, "snmpwalk", "timeResultsSingle",
    //                            NETSNMP_DS_APPLICATION_ID,
    //                            NETSNMP_DS_WALK_TIME_RESULTS_SINGLE);
    struct timeval tv1, tv2, tv_a, tv_b;
    netsnmp_variable_list *vars;
    for (hp = hosts; hp->name; hp++)
    {
        struct snmp_session ss, *sp;
        struct oid *op;
        snmp_sess_init(&ss); /* initialize session */
        ss.version = SNMP_VERSION_2c;
        ss.peername = strdup(hp->name);
        ss.community = strdup(hp->community);
        ss.community_len = strlen(ss.community);
        if (!(sp = snmp_open(&ss)))
        {
            snmp_perror("snmp_open");
            continue;
        }

        for (op = oids; op->Name; op++)
        {
            running = 1;
            rootlen = MAX_OID_LEN;
            fprintf(stderr, "op->Name[arg]:%s\n", op->Name);
            if (snmp_parse_oid(op->Name, root, &rootlen) == NULL)
            {
                printf("111111\n");
                snmp_perror(op->Name);
                exit(1);
            }
            {
                printf("root:%s\n", root);
                memmove(end_oid, root, rootlen * sizeof(oid));
                end_len = rootlen;
                end_oid[end_len - 1]++;
            }

            struct snmp_pdu *req, *resp;
            int status;
            int check = !netsnmp_ds_get_boolean(NETSNMP_DS_APPLICATION_ID,
                                                NETSNMP_DS_WALK_DONT_CHECK_LEXICOGRAPHIC);
            if (netsnmp_ds_get_boolean(NETSNMP_DS_APPLICATION_ID, NETSNMP_DS_WALK_INCLUDE_REQUESTED))
            {
                snmp_get_and_print(&ss, root, rootlen);
                printf("111111\n");
            }

            if (netsnmp_ds_get_boolean(NETSNMP_DS_APPLICATION_ID,
                                       NETSNMP_DS_WALK_TIME_RESULTS))
                netsnmp_get_monotonic_clock(&tv1);

            memmove(name, root, rootlen * sizeof(oid));
            name_length = rootlen;

            while (running)
            {
                req = snmp_pdu_create(SNMP_MSG_GETNEXT);
                snmp_add_null_var(req, name, name_length);
                fprintf(stderr, "name:%s\tname_length:%d\n", (char *)name, name_length);
                status = snmp_synch_response(sp, req, &resp);
                if (status == STAT_SUCCESS)
                {
                    if (netsnmp_ds_get_boolean(NETSNMP_DS_APPLICATION_ID, NETSNMP_DS_WALK_TIME_RESULTS_SINGLE))
                        netsnmp_get_monotonic_clock(&tv_b);
                    if (resp->errstat == SNMP_ERR_NOERROR)
                    {
                        /*
                         * check resulting variables
                         */
                        for (vars = resp->variables; vars;
                             vars = vars->next_variable)
                        {
                            printf("for (vars = resp->variables...\n");
                            if (snmp_oid_compare(end_oid, end_len,
                                                 vars->name, vars->name_length) <= 0)
                            {
                                /*
                                 * not part of this subtree
                                 */
                                running = 0;
                                continue;
                            }
                            if (netsnmp_ds_get_boolean(NETSNMP_DS_APPLICATION_ID, NETSNMP_DS_WALK_TIME_RESULTS_SINGLE))
                                fprintf(stdout, "%f s: ",
                                        (double)(tv_b.tv_usec - tv_a.tv_usec) / 1000000 +
                                            (double)(tv_b.tv_sec - tv_a.tv_sec));
                            print_variable(vars->name, vars->name_length, vars);
                            if ((vars->type != SNMP_ENDOFMIBVIEW) &&
                                (vars->type != SNMP_NOSUCHOBJECT) &&
                                (vars->type != SNMP_NOSUCHINSTANCE))
                            {
                                /*
                                 * not an exception value
                                 */
                                if (check && snmp_oid_compare(name, name_length,
                                                              vars->name,
                                                              vars->name_length) >= 0)
                                {
                                    fflush(stdout);
                                    fprintf(stderr, "Error: OID not increasing: ");
                                    fprint_objid(stderr, name, name_length);
                                    fprintf(stderr, " >= ");
                                    fprint_objid(stderr, vars->name,
                                                 vars->name_length);
                                    fprintf(stderr, "\n");
                                    running = 0;
                                }
                                fprintf(stderr, "vars->name:%s\n", (char *)vars->name);
                                memmove((char *)name, (char *)vars->name,
                                        vars->name_length * sizeof(oid));
                                name_length = vars->name_length;
                            }
                            else
                                /*
                                 * an exception value, so stop
                                 */
                                running = 0;
                        }
                    }
                    else
                    {
                        /*
                         * error in resp, print it
                         */
                        running = 0;
                        if (resp->errstat == SNMP_ERR_NOSUCHNAME)
                        {
                            printf("End of MIB\n");
                        }
                        else
                        {
                            fprintf(stderr, "Error in packet.\nReason: %s\n",
                                    snmp_errstring(resp->errstat));
                            if (resp->errindex != 0)
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
                            ss.peername);
                    running = 0;
                }
                else
                { /* status == STAT_ERROR */
                    snmp_sess_perror("snmpwalk", &ss);
                    running = 0;
                }
                if (resp)
                    snmp_free_pdu(resp);
            }
        }
        snmp_close(sp);
    }
}

/*****************************************************************************/

int main(int argc, char **argv)
{
    printf("netsnmp_get_version:%s\n", netsnmp_get_version());
    initialize();

    printf("---------- synchronous -----------\n");
    synchronous();
    return 0;
}