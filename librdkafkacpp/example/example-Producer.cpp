#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <cstring>
#include <fstream>
#include <getopt.h>

#include "rdkafkacpp.h"
using namespace std;
static bool run = true;

static void sigterm(int sig)
{
    run = false;
}

class ExampleDeliveryReportCb : public RdKafka::DeliveryReportCb
{
public:
    void dr_cb(RdKafka::Message &message)
    {
        std::cout << "Message delivery for (" << message.len() << " bytes): " << message.errstr() << std::endl;
        if (message.key())
            std::cout << "Key: " << *(message.key()) << ";" << std::endl;
    }
};

class ExampleEventCb : public RdKafka::EventCb
{
public:
    void event_cb(RdKafka::Event &event)
    {
        switch (event.type())
        {
        case RdKafka::Event::EVENT_ERROR:
            std::cerr << "ERROR (" << RdKafka::err2str(event.err()) << "): " << event.str() << std::endl;
            if (event.err() == RdKafka::ERR__ALL_BROKERS_DOWN)
                run = false;
            break;

        case RdKafka::Event::EVENT_STATS:
            std::cerr << "\"STATS\": " << event.str() << std::endl;
            break;

        case RdKafka::Event::EVENT_LOG:
            fprintf(stderr, "LOG-%i-%s: %s\n",
                    event.severity(), event.fac().c_str(), event.str().c_str());
            break;

        default:
            std::cerr << "EVENT " << event.type() << " (" << RdKafka::err2str(event.err()) << "): " << event.str() << std::endl;
            break;
        }
    }
};

int main()
{
    std::string brokers = "127.0.0.1";
    std::string errstr;
    std::string topic_str = "test";
    int32_t partition = RdKafka::Topic::PARTITION_UA;

    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    RdKafka::Conf *tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

    if (conf->set("bootstrap.servers", brokers, errstr) == RdKafka::Conf::CONF_OK)
        fprintf(stderr, "%s=%s\n", "bootstrap.servers", brokers.c_str());
    else
        fprintf(stderr, "ERR[%s]:%s=%s\n", errstr.c_str(), "bootstrap.servers", brokers.c_str());

    if (conf->set("security.protocol", "SASL_PLAINTEXT", errstr) == RdKafka::Conf::CONF_OK)
        fprintf(stderr, "%s=%s\n", "security.protocol", "SASL_PLAINTEXT");
    else
        fprintf(stderr, "ERR[%s]:%s=%s\n", errstr.c_str(), "security.protocol", "SASL_PLAINTEXT");

    if (conf->set("sasl.kerberos.keytab", "/var/kerberos/krb5kdc/kadm5.keytab", errstr) == RdKafka::Conf::CONF_OK)
        fprintf(stderr, "%s=%s\n", "sasl.kerberos.keytab", "/var/kerberos/krb5kdc/kadm5.keytab");
    else
        fprintf(stderr, "ERR[%s]:%s=%s\n", errstr.c_str(), "sasl.kerberos.keytab", "/var/kerberos/krb5kdc/kadm5.keytab");

    if (conf->set("sasl.kerberos.principal", "kafka/centos7-6@HADOOP.COM", errstr) == RdKafka::Conf::CONF_OK)
        fprintf(stderr, "%s=%s\n", "sasl.kerberos.principal", "kafka/centos7-6@HADOOP.COM");
    else
        fprintf(stderr, "ERR[%s]:%s=%s\n", errstr.c_str(), "sasl.kerberos.principal", "kafka/centos7-6@HADOOP.COM");

    ExampleEventCb ex_event_cb;
    conf->set("event_cb", &ex_event_cb, errstr);

    signal(SIGINT, sigterm);
    signal(SIGTERM, sigterm);

    ExampleDeliveryReportCb ex_dr_cb;
    conf->set("dr_cb", &ex_dr_cb, errstr);

    RdKafka::Producer *producer = RdKafka::Producer::create(conf, errstr);
    if (!producer)
    {
        std::cerr << "Failed to create producer: " << errstr << std::endl;
        exit(1);
    }
    std::cout << "% Created producer " << producer->name() << std::endl;

    RdKafka::Topic *topic = RdKafka::Topic::create(producer, topic_str,
                                                   tconf, errstr);
    if (!topic)
    {
        std::cerr << "Failed to create topic: " << errstr << std::endl;
        exit(1);
    }

    // 退出要摁 Ctrl+C + 回车
    for (std::string line; run && std::getline(std::cin, line);)
    {
        if (line.empty())
        {
            producer->poll(0);
            continue;
        }

        RdKafka::ErrorCode resp =
            producer->produce(topic, partition,
                              RdKafka::Producer::RK_MSG_COPY /* Copy payload */,
                              const_cast<char *>(line.c_str()), line.size(),
                              NULL, NULL);
        if (resp != RdKafka::ERR_NO_ERROR)
            std::cerr << "% Produce failed: " << RdKafka::err2str(resp) << std::endl;
        else
            std::cerr << "% Produced message (" << line.size() << " bytes)" << std::endl;

        producer->poll(0);
    }

    run = true;
    // 退出前处理完输出队列中的消息
    while (run && producer->outq_len() > 0)
    {
        std::cerr << "Waiting for " << producer->outq_len() << std::endl;
        std::cerr << "producer->outq_len()= " << producer->outq_len() << std::endl;
        producer->poll(1000);
    }

    delete conf;
    delete tconf;
    delete topic;
    delete producer;

    RdKafka::wait_destroyed(5000);

    return 0;
}