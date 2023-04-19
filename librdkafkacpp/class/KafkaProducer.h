#ifndef __KAFKA_PRODUCER__H__
#define __KAFKA_PRODUCER__H__

#include <iostream>
#include <string>
#include <map>
#include <stdio.h>

#include "rdkafkacpp.h"

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

typedef std::map<std::string, RdKafka::Topic *> TopicMap;

class KafkaProducer
{
public:
    KafkaProducer(/* args */);
    ~KafkaProducer();

    bool Init(std::string configFile);

    void Send(const char *data, size_t data_len);

    inline int outq_len()
    {
        return m_pProducer->outq_len();
    }

    inline void poll(int timeout_ms)
    {
        m_pProducer->poll(timeout_ms);
    }

private:
    bool LoadCfg(const char *cfg_file, RdKafka::Conf *conf, RdKafka::Conf *tconf, std::vector<std::string> &topics);

private:
    TopicMap m_TopicMap;
    RdKafka::Producer *m_pProducer;

    ExampleDeliveryReportCb m_ex_dr_cb;
    ExampleEventCb m_ex_event_cb;
};

#endif