#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "MQTTCli.h"
#include "OneNET.h"

MQTTCli mqttClient;

char *msg = "{\"id\":\"123\",\"version\":\"1.0\",\"params\":{\"w10001\":{\"value\":22.2}}}";

using namespace std;

void onConnectFailureClient(void *context, MQTTAsync_failureData *response)
{
    mqttClient.m_connected = 0;
    printf("mqtt:connect failed, rc %d\n", response ? response->code : -1);
    MQTTAsync client = (MQTTAsync)context;
}

void onConnectClient(void *context, MQTTAsync_successData *response)
{
    mqttClient.m_connected = 1;
    //连接成功的回调，只会在第一次 connect 成功后调用，后续自动重连成功时并不会调用，因此应用需要自行保证每次 connect 成功后重新订阅
    printf("mqtt:client:connect success\n");

    mqttClient.m_connected = 1;
    mqttClient.publish_send(msg);
}

void onDisconnectClient(void *context, MQTTAsync_successData *response)
{
    mqttClient.m_connected = 0;
    printf("mqtt:connect disconnect\n");
}

void onPublishFailureClient(void *context, MQTTAsync_failureData *response)
{
    printf("Publish failed, rc %d\n", response ? -1 : response->code);
}

void onPublishClient(void *context, MQTTAsync_successData *response)
{
    printf("mqtt:publish:send success\n");
}

void connectionLostClient(void *context, char *cause)
{
    mqttClient.m_connected = 0;
    printf("mqtt:client:connection lost\n");
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

    mqttClient.onConnectFailure = onConnectFailureClient;
    mqttClient.onConnectClient = onConnectClient;
    mqttClient.onDisconnect = onDisconnectClient;
    mqttClient.onPublishFailure = onPublishFailureClient;
    mqttClient.onPublish = onPublishClient;
    mqttClient.connectionLost = connectionLostClient;

    mqttClient.init("183.230.102.116", 1883, "mqtt_test_2022_06_d1",
                    "$sys/ux9jchZp8s/mqtt_test_2022_06_d1/thing/property/post", "ux9jchZp8s", token);

    while (true)
    {
        sleep(1);
        if (mqttClient.publish_connect())
        {
            sleep(1);
            continue;
        }

        sleep(10);
        mqttClient.m_connected = 0;
        mqttClient.publish_disconnect();
    }

    return 0;
}