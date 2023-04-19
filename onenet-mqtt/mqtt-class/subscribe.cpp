#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "MQTTCli.h"
#include "OneNET.h"

MQTTCli mqttServer;

using namespace std;

int messageArrivedServer(void *context, char *topicName, int topicLen, MQTTAsync_message *m)
{
    printf("recv message from %s ,body is %s\n", topicName, (char *)m->payload);

    // TODO::在此处增加接收内容解析程序

    MQTTAsync_freeMessage(&m);
    MQTTAsync_free(topicName);
    return 1;
}

void onConnectFailureServer(void *context, MQTTAsync_failureData *response)
{
    if (mqttServer.m_connected)
        mqttServer.m_connected = 0;

    printf("mqtt:connect failed, rc %d\n", response ? response->code : -1);
    MQTTAsync client = (MQTTAsync)context;
}

void onSubcribeServer(void *context, MQTTAsync_successData *response)
{
    printf("subscribe success\n");
}

void onConnectServer(void *context, MQTTAsync_successData *response)
{
    if (!mqttServer.m_connected)
        mqttServer.m_connected = 1;

    //连接成功的回调，只会在第一次 connect 成功后调用，后续自动重连成功时并不会调用，因此应用需要自行保证每次 connect 成功后重新订阅
    printf("mqtt:server:connect success\n");
    MQTTAsync client = (MQTTAsync)context;
    // do sub when connect success
    MQTTAsync_responseOptions sub_opts = MQTTAsync_responseOptions_initializer;
    sub_opts.onSuccess = mqttServer.onSubcribe;
    int rc = 0;
    if ((rc = MQTTAsync_subscribe(client, mqttServer.m_topic.c_str(), 1, &sub_opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to subscribe, return code %d\n", rc);
    }
}

void onDisconnectServer(void *context, MQTTAsync_successData *response)
{
    if (mqttServer.m_connected)
        mqttServer.m_connected = 0;

    printf("mqtt:connect disconnect\n");
}

void onPublishFailureServer(void *context, MQTTAsync_failureData *response)
{
    printf("Publish failed, rc %d\n", response ? -1 : response->code);
}

void connectionLostServer(void *context, char *cause)
{
    if (mqttServer.m_connected)
        mqttServer.m_connected = 0;

    printf("mqtt:server:connection lost\n");
}

int main()
{
    OneNET onenet;
    onenet.key = "axnHhlFmJvxLM8ZCPFzmM5antrxOUuu/L5W9YzmXUm4=";
    onenet.res = "products/ux9jchZp8s/devices/mqtt_test_2022_06_d1";
    onenet.et = "2547098723";
    onenet.method = "sha1";
    onenet.version = "2018-10-31";
    string token = onenet.Token();

    mqttServer.messageArrived = messageArrivedServer;
    mqttServer.onConnectFailure = onConnectFailureServer;
    mqttServer.onSubcribe = onSubcribeServer;
    mqttServer.onConnectServer = onConnectServer;
    mqttServer.onDisconnect = onDisconnectServer;
    mqttServer.onPublishFailure = onPublishFailureServer;
    mqttServer.connectionLost = connectionLostServer;

    mqttServer.init("183.230.102.116", 1883, "mqtt_test_2022_06_d1",
                    "$sys/ux9jchZp8s/mqtt_test_2022_06_d1/thing/property/post/reply", "ux9jchZp8s", token);

    while (true)
    {
        if (mqttServer.subscribe_connect())
        {
            sleep(1);
            continue;
        }
        break;
    }
    while (true)
    {
        sleep(2);
    }
    mqttServer.subscribe_disconnect();
    return 0;
}