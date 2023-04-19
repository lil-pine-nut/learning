/*
 * =============================================================================
 *
 *       Filename:  QueryServer.c
 *
 *    Description:  QueryServer
 *
 *        Created:  02/15/2013 08:48:49 PM
 *
 *         Author:  Fu Haiping (forhappy), haipingf@gmail.com
 *        Company:  ICT ( Institute Of Computing Technology, CAS )
 *
 * =============================================================================
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <zookeeper/zookeeper.h>
#include <zookeeper/zookeeper_log.h>

void QueryServer_watcher_g(zhandle_t *zh, int type, int state,
                           const char *path, void *watcherCtx)
{
    if (type == ZOO_SESSION_EVENT)
    {
        if (state == ZOO_CONNECTED_STATE)
        {
            printf("[[[QueryServer]]] Connected to zookeeper service successfully!\n");
        }
        else if (state == ZOO_EXPIRED_SESSION_STATE)
        {
            printf("Zookeeper session expired!\n");
        }
    }
}

void QueryServer_string_completion(int rc, const char *name, const void *data)
{
    fprintf(stderr, "[%s]: rc = %d\n", (char *)(data == 0 ? "null" : data), rc);
    if (!rc)
    {
        fprintf(stderr, "\tname = %s\n", name);
    }
}

void QueryServer_accept_query()
{
    printf("QueryServer is running...\n");
}

int main(int argc, const char *argv[])
{
    const char *host = "127.0.0.1:2185";
    int timeout = 30000;

    zoo_set_debug_level(ZOO_LOG_LEVEL_WARN);
    zhandle_t *zkhandle = zookeeper_init(host,
                                         QueryServer_watcher_g, timeout, 0, (void *)"hello zookeeper.", 0);
    if (zkhandle == NULL)
    {
        fprintf(stderr, "Error when connecting to zookeeper servers...\n");
        exit(EXIT_FAILURE);
    }

    // struct ACL ALL_ACL[] = {{ZOO_PERM_ALL, ZOO_ANYONE_ID_UNSAFE}};
    // struct ACL_vector ALL_PERMS = {1, ALL_ACL};
    int ret = zoo_acreate(zkhandle, "/QueryServer", "alive", 5,
                          &ZOO_OPEN_ACL_UNSAFE, ZOO_EPHEMERAL,
                          QueryServer_string_completion, "zoo_acreate");
    if (ret)
    {
        fprintf(stderr, "Error %d for %s\n", ret, "acreate");
        exit(EXIT_FAILURE);
    }

    do
    {
        // 模拟 QueryServer 对外提供服务.
        // 为了简单起见, 我们在此调用一个简单的函数来模拟 QueryServer.
        // 然后休眠 5 秒，程序主动退出(即假设此时已经崩溃).
        QueryServer_accept_query();
        sleep(5);
    } while (false);

    zookeeper_close(zkhandle);
}