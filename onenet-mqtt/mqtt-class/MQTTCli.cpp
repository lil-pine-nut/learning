#include <memory.h>
#include <stdlib.h>
#include <unistd.h>
#include "MQTTCli.h"

MQTTCli::MQTTCli()
{
    m_connected = 0;
}

void MQTTCli::init(string host, int port, string clientid, string topic,
                   string username, string password, string ca_certificate_file)
{
    m_caCertificateFile = ca_certificate_file;
    char url[128];
    memset(url, 0, sizeof(url));
    if (!m_caCertificateFile.empty())
    {
        snprintf(url, sizeof(url), "ssl://%s:%d", host.c_str(), port);
    }
    else
    {
        snprintf(url, sizeof(url), "tcp://%s:%d", host.c_str(), port);
    }
    m_url = url;
    m_clientID = clientid;
    m_topic = topic;
    m_userName = username;
    m_passWord = password;
}

int MQTTCli::subscribe_connect()
{
    int cleanSession = 1;
    int rc = 0;
    // 1.create m_client
    MQTTAsync_createOptions create_opts = MQTTAsync_createOptions_initializer;
    create_opts.sendWhileDisconnected = 0;
    create_opts.maxBufferedMessages = 10;

    rc = MQTTAsync_createWithOptions(&m_client, m_url.c_str(), m_clientID.c_str(), MQTTCLIENT_PERSISTENCE_NONE, NULL, &create_opts);
    rc = MQTTAsync_setCallbacks(m_client, m_client, connectionLost, messageArrived, NULL);
    // 2.connect to server
    MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
    conn_opts.MQTTVersion = MQTTVERSION_3_1_1;
    conn_opts.keepAliveInterval = 60;
    conn_opts.cleansession = cleanSession;
    conn_opts.username = m_userName.c_str();
    conn_opts.password = m_passWord.c_str();
    conn_opts.onSuccess = onConnectServer;
    conn_opts.onFailure = onConnectFailure;
    conn_opts.context = m_client;
    //如果需要使用 SSL 加密
    if (!m_caCertificateFile.empty())
    {
        MQTTAsync_SSLOptions ssl = MQTTAsync_SSLOptions_initializer;
        conn_opts.ssl = &ssl;
        conn_opts.ssl->trustStore = m_caCertificateFile.c_str();
        conn_opts.ssl->sslVersion = MQTT_SSL_VERSION_TLS_1_2;
    }
    else
    {
        conn_opts.ssl = NULL;
    }
    conn_opts.automaticReconnect = 1;
    conn_opts.connectTimeout = 3;
    if ((rc = MQTTAsync_connect(m_client, &conn_opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start connect, return code %d\n", rc);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int MQTTCli::subscribe_disconnect()
{
    int rc = 0;
    MQTTAsync_disconnectOptions disc_opts = MQTTAsync_disconnectOptions_initializer;
    disc_opts.onSuccess = onDisconnect;
    if ((rc = MQTTAsync_disconnect(m_client, &disc_opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start disconnect, return code %d\n", rc);
        return EXIT_FAILURE;
    }
    while (m_connected)
        sleep(1);
    MQTTAsync_destroy(&m_client);
    return EXIT_SUCCESS;
}

int MQTTCli::publish_connect(void)
{
    int cleanSession = 1;
    int rc = 0;
    // 1.create m_client
    MQTTAsync_createOptions create_opts = MQTTAsync_createOptions_initializer;
    create_opts.sendWhileDisconnected = 0;
    create_opts.maxBufferedMessages = 10;
    rc = MQTTAsync_createWithOptions(&m_client, m_url.c_str(), m_clientID.c_str(), MQTTCLIENT_PERSISTENCE_NONE, NULL, &create_opts);
    rc = MQTTAsync_setCallbacks(m_client, m_client, connectionLost, messageArrived, NULL);
    // 2.connect to server
    MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
    conn_opts.MQTTVersion = MQTTVERSION_3_1_1;
    conn_opts.keepAliveInterval = 60;
    conn_opts.cleansession = cleanSession;
    conn_opts.username = m_userName.c_str();
    conn_opts.password = m_passWord.c_str();
    conn_opts.onSuccess = onConnectClient;
    conn_opts.onFailure = onConnectFailure;
    conn_opts.context = m_client;
    //如果需要使用 SSL 加密
    if (!m_caCertificateFile.empty())
    {
        MQTTAsync_SSLOptions ssl = MQTTAsync_SSLOptions_initializer;
        conn_opts.ssl = &ssl;
        conn_opts.ssl->trustStore = m_caCertificateFile.c_str();
        conn_opts.ssl->sslVersion = MQTT_SSL_VERSION_TLS_1_2;
    }
    else
    {
        conn_opts.ssl = NULL;
    }
    conn_opts.automaticReconnect = 1;
    conn_opts.connectTimeout = 3;
    if ((rc = MQTTAsync_connect(m_client, &conn_opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start connect, return code %d\n", rc);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int MQTTCli::publish_send(char *msg)
{
    int rc = 0;

    // 3.publish msg
    MQTTAsync_responseOptions pub_opts = MQTTAsync_responseOptions_initializer;
    pub_opts.onSuccess = onPublish;
    pub_opts.onFailure = onPublishFailure;
    rc = MQTTAsync_send(m_client, m_topic.c_str(), strlen(msg), msg, 0, 0, &pub_opts);
    if (rc != MQTTASYNC_SUCCESS)
    {
        printf("mqtt:publish:failed:%s\n", msg);
        return EXIT_FAILURE;
    }
    else
    {
        printf("mqtt:publish:success:%s\n", msg);
        return EXIT_SUCCESS;
    }
}

int MQTTCli::publish_disconnect(void)
{
    int rc = 0;
    MQTTAsync_disconnectOptions disc_opts = MQTTAsync_disconnectOptions_initializer;
    disc_opts.onSuccess = onDisconnect;
    if ((rc = MQTTAsync_disconnect(m_client, &disc_opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start disconnect, return code %d\n", rc);
        return EXIT_FAILURE;
    }
    while (m_connected)
        sleep(1);
    MQTTAsync_destroy(&m_client);
    return EXIT_SUCCESS;
}