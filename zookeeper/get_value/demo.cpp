#include <stdio.h>

#include <errno.h>
#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <assert.h>

#define THREADED
#include <zookeeper/zookeeper.h>

using namespace std;

const char *node_path = "/home/lws/zookeeper/spark";

static void config_mg_awexists(zhandle_t *zh);

static void dump_stat(const struct Stat *stat)
{
    char tctimes[40];
    char tmtimes[40];
    time_t tctime;
    time_t tmtime;

    if (!stat)
    {
        fprintf(stderr, "\tnull\n");
        return;
    }
    tctime = stat->ctime / 1000;
    tmtime = stat->mtime / 1000;

    ctime_r(&tmtime, tmtimes);
    ctime_r(&tctime, tctimes);

    fprintf(stderr, "\tctime = %s\tczxid=%llx\n"
                    "\tmtime=%s\tmzxid=%llx\n"
                    "\tversion=%x\taversion=%x\n"
                    "\tephemeralOwner = %llx\n"
                    "\tdataLength = %d\n"
                    "\tnumChildren = %d\n",
            tctimes, stat->czxid,
            tmtimes, stat->mzxid,
            (unsigned int)stat->version, (unsigned int)stat->aversion,
            stat->ephemeralOwner, stat->dataLength, stat->numChildren);
}

static bool is_exists(zhandle_t *zh, const char *path)
{
    struct Stat stat = {0};
    int watch;
    int ret = zoo_exists(zh, path, watch, &stat);
    // cerr << "watch = " << watch << endl;
    dump_stat(&stat);
    if (ret != ZOK)
        cerr << "zoo_exists ret = " << ret << endl;
    if (ret == ZOK)
        return true;
    else if (ret == ZNONODE)
        return false;
    else if (ret == ZCONNECTIONLOSS)
        return false;
    else
    {
        fprintf(stderr, "Error %d for %s\n", ret, "aexists");
        exit(EXIT_FAILURE);
    }
    return true;
}

void config_mg_watcher_g(zhandle_t *zh, int type, int state,
                         const char *path, void *watcherCtx)
{
    if (type == ZOO_SESSION_EVENT)
    {
        if (state == ZOO_CONNECTED_STATE)
        {
            printf("[[[config_mg]]] Connected to zookeeper service successfully!\n");
        }
        else if (state == ZOO_EXPIRED_SESSION_STATE)
        {
            printf("Zookeeper session expired!\n");
        }
    }
}

static string get_node_value(zhandle_t *zh, const char *path)
{
    int watch;
    char read_buff[65536] = {0};
    int buf_len = sizeof(read_buff); //输入的buff长度应该为buff的长度，为0读取不了数据
    struct Stat stat = {0};
    int ret = zoo_get(zh, path, watch, read_buff, &buf_len, &stat);
    // dump_stat(&stat);
    if (ret)
    {
        fprintf(stderr, "Error %d for %s\n", ret, "zoo_get");
        exit(EXIT_FAILURE);
    }
    assert(stat.dataLength <= sizeof(read_buff) && "stat.dataLength > sizeof(read_buff)");
    return string(read_buff, buf_len);
}

void config_mg_watcher_awexists(zhandle_t *zh, int type, int state,
                                const char *path, void *watcherCtx)
{
    fprintf(stderr, "%s:%d\t type = %d, state = %d\n", __FUNCTION__, __LINE__, type, state);
    if (state == ZOO_CONNECTED_STATE)
    {
        if (type == ZOO_DELETED_EVENT)
        {
            printf("\tdelete %s ...\n", path);
        }
        else if (type == ZOO_CREATED_EVENT)
        {
            printf("\tcreate %s ...\n", path);
        }
        else if (type == ZOO_CHANGED_EVENT)
        {
            string value = get_node_value(zh, path);
            printf("\tchange %s  --  change-value:%s \n", path, value.c_str());
            printf("\tnotify all client to update DB_Conn_Config ...\n");
        }
    }

    // re-exists and set watch on /commConfig again.
    config_mg_awexists(zh);
}

void config_mg_stat_completion(int rc, const struct Stat *stat,
                               const void *data)
{
    // fprintf(stderr, "%s:%d\t%s: rc = %d Stat:\n", __func__ , __LINE__, (char *) data, rc);
    // dump_stat(stat);
}

static void config_mg_awexists(zhandle_t *zh)
{
    int ret = zoo_awexists(zh, node_path,
                           config_mg_watcher_awexists,
                           (void *)"_awexists.",
                           config_mg_stat_completion,
                           "zoo_awexists");
    if (ret)
    {
        fprintf(stderr, "Error %d for %s\n", ret, "zoo_awexists");
        exit(EXIT_FAILURE);
    }
}

int main()
{
    const char *host = "127.0.0.1:2185,127.0.0.1:2185";
    int timeout = 30000;

    zoo_set_debug_level(ZOO_LOG_LEVEL_WARN);
    zhandle_t *zkhandle = zookeeper_init(host,
                                         config_mg_watcher_g, timeout, 0, (void *)"hello zookeeper.", 0);
    if (zkhandle == NULL)
    {
        fprintf(stderr, "Error when connecting to zookeeper servers...\n");
        exit(EXIT_FAILURE);
    }

    struct ACL ALL_ACL[] = {{ZOO_PERM_ALL, ZOO_ANYONE_ID_UNSAFE}};
    struct ACL_vector ALL_PERMS = {1, ALL_ACL};
    config_mg_awexists(zkhandle);

    if (is_exists(zkhandle, node_path))
    {
        // 获取节点信息

        int watch;

        // 同步 - zoo_get
        char read_buff[65536] = {0};
        int buf_len = sizeof(read_buff); //输入的buff长度应该为buff的长度，为0读取不了数据
        struct Stat stat = {0};
        int ret = zoo_get(zkhandle, node_path, watch, read_buff, &buf_len, &stat);
        // dump_stat(&stat);
        if (ret)
        {
            fprintf(stderr, "Error %d for %s\n", ret, "zoo_get");
            exit(EXIT_FAILURE);
        }
        assert(stat.dataLength <= sizeof(read_buff) && "stat.dataLength > sizeof(read_buff)");
        // std::cout << "buf_len = " << buf_len << endl;
        // std::cout << "read_buff = " << read_buff << endl;
        string config_str2 = string(read_buff, buf_len);
        cerr << "zoo_get " << node_path << "  get-value:" << config_str2 << endl;

        // // 异步 - zoo_aget
        // int ret2 = zoo_aget(zkhandle, node_path, watch, config_mg_get_data_completion, NULL);
        // if (ret2) {
        //     fprintf(stderr, "Error %d for %s\n", ret2, "zoo_aget");
        //     exit(EXIT_FAILURE);
        // }
        struct String_vector string_vector
        {
            0
        };
        ret = zoo_get_children(zkhandle, node_path, watch, &string_vector);
        if (ret)
        {
            fprintf(stderr, "Error %d for %s\n", ret, "zoo_get");
            exit(EXIT_FAILURE);
        }
        string children_node;
        cerr << "string_vector.count = " << string_vector.count << endl;
        for (int i = 0; i < string_vector.count; i++)
        {
            cerr << "[" << i << "] = " << string_vector.data[i] << endl;
            children_node = string(node_path) + string_vector.data[i];
        }

        deallocate_String_vector(&string_vector);
    }
    else
    {
        cerr << node_path << " not exit!" << endl;
    }

    do
    {
        printf("sleep(1) ...\n");
        sleep(1);
    } while (false);

    // // 删除节点
    // if(is_exists(zkhandle, node_path))
    // {
    //     int ret = zoo_delete(zkhandle, node_path, -1);
    //     if (ret) {
    //         fprintf(stderr, "Error %d for %s\n", ret, "zoo_delete");
    //         exit(EXIT_FAILURE);
    //     }
    //     cerr << "zoo_delete " << node_path << "." << endl;
    // }

    printf("摁回车键结束......\n");
    getchar();

    zookeeper_close(zkhandle);
}