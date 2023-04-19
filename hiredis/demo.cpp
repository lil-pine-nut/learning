#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hiredis.h"
#include "win32.h"

int main(int argc, char **argv)
{
    unsigned int j, isunix = 0;
    redisContext *c;
    redisReply *reply;
    const char *hostname = "127.0.0.1";

    int port = 6379;

    struct timeval timeout = {1, 500000}; // 1.5 seconds
    if (isunix)
    {
        c = redisConnectUnixWithTimeout(hostname, timeout);
    }
    else
    {
        c = redisConnectWithTimeout(hostname, port, timeout);
    }
    if (c == NULL || c->err)
    {
        if (c)
        {
            printf("Connection error: %s\n", c->errstr);
            redisFree(c);
        }
        else
        {
            printf("Connection error: can't allocate redis context\n");
        }
        exit(1);
    }

    /* PING server */
    reply = (redisReply *)redisCommand(c, "PING");
    printf("PING: %s\n", reply->str);
    freeReplyObject(reply);

    // /* Set a key */
    // reply = redisCommand(c,"SET %s %s", "foo", "hello world");
    // printf("SET: %s\n", reply->str);
    // freeReplyObject(reply);

    // /* Set a key using binary safe API */
    // reply = redisCommand(c,"SET %b %b", "bar", (size_t) 3, "hello", (size_t) 5);
    // printf("SET (binary API): %s\n", reply->str);
    // freeReplyObject(reply);

    // /* Set a key using binary safe API */
    // reply = redisCommand(c,"SET %b %b", "bar", (size_t) 3, "hello", (size_t) 5);
    // printf("SET (binary API): %s\n", reply->str);
    // freeReplyObject(reply);

    /* Try a GET and two INCR */
    reply = (redisReply *)redisCommand(c, "GET UeCnt");
    printf("GET foo: %s\n", reply->str);
    freeReplyObject(reply);

    /* Try a GET and two INCR */
    reply = (redisReply *)redisCommand(c, "GET Speed");
    printf("GET foo: %s\n", reply->str);
    freeReplyObject(reply);

    /* Try a GET and two INCR */
    reply = (redisReply *)redisCommand(c, "GET SpeedMax");
    printf("GET foo: %s\n", reply->str);
    freeReplyObject(reply);

    // reply = redisCommand(c,"INCR counter");
    // printf("INCR counter: %lld\n", reply->integer);
    // freeReplyObject(reply);
    // /* again ... */
    // reply = redisCommand(c,"INCR counter");
    // printf("INCR counter: %lld\n", reply->integer);
    // freeReplyObject(reply);

    // /* Create a list of numbers, from 0 to 9 */
    // reply = redisCommand(c,"DEL mylist");
    // freeReplyObject(reply);
    // for (j = 0; j < 10; j++) {
    //     char buf[64];

    //     snprintf(buf,64,"%u",j);
    //     reply = redisCommand(c,"LPUSH mylist element-%s", buf);
    //     freeReplyObject(reply);
    // }

    // /* Let's check what we have inside the list */
    // reply = redisCommand(c,"LRANGE mylist 0 -1");
    // if (reply->type == REDIS_REPLY_ARRAY) {
    //     for (j = 0; j < reply->elements; j++) {
    //         printf("%u) %s\n", j, reply->element[j]->str);
    //     }
    // }
    // freeReplyObject(reply);

    /* Disconnects and frees the context */
    redisFree(c);

    return 0;
}
