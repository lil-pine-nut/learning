#ifndef __MQTT__CHI__H__
#define __MQTT__CHI__H__

#include "MQTTAsync.h"
#include <string>

using namespace std;

class MQTTCli
{
public:
    int (*messageArrived)(void *context, char *topicName, int topicLen, MQTTAsync_message *m);
    void (*onConnectFailure)(void *context, MQTTAsync_failureData *response);
    void (*onSubcribe)(void *context, MQTTAsync_successData *response);
    void (*onConnectServer)(void *context, MQTTAsync_successData *response);
    void (*onConnectClient)(void *context, MQTTAsync_successData *response);
    void (*onDisconnect)(void *context, MQTTAsync_successData *response);
    void (*onPublishFailure)(void *context, MQTTAsync_failureData *response);
    void (*onPublish)(void *context, MQTTAsync_successData *response);
    void (*connectionLost)(void *context, char *cause);

    MQTTCli();

    void init(string host, int port, string clientid, string topic,
              string username, string password, string ca_certificate_file = "");
    int subscribe_connect();
    int subscribe_disconnect(void);
    int publish_connect();
    int publish_send(char *msg);
    int publish_disconnect(void);

    int m_connected;
    string m_topic;

private:
    string m_caCertificateFile;

    //测试收发消息的 Topic
    string m_clientID;
    string m_userName;
    string m_passWord;
    string m_url;

    MQTTAsync m_client;
};

#endif