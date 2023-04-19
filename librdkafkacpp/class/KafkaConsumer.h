#ifndef __KAFKA_CONSUMER__H__
#define __KAFKA_CONSUMER__H__

#include "rdkafkacpp.h"
#include <iostream>
#include <stdio.h>

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
                break;

        case RdKafka::Event::EVENT_STATS:
            std::cerr << "\"STATS\": " << event.str() << std::endl;
            break;

        case RdKafka::Event::EVENT_LOG:
            fprintf(stderr, "LOG-%i-%s: %s\n",
                    event.severity(), event.fac().c_str(), event.str().c_str());
            break;

        case RdKafka::Event::EVENT_THROTTLE:
            std::cerr << "THROTTLED: " << event.throttle_time() << "ms by " << event.broker_name() << " id " << (int)event.broker_id() << std::endl;
            break;

        default:
            std::cerr << "EVENT " << event.type() << " (" << RdKafka::err2str(event.err()) << "): " << event.str() << std::endl;
            break;
        }
    }
};

class KafkaConsumer
{
public:
    KafkaConsumer(/* args */);
    ~KafkaConsumer();

    bool Init(std::string configFile);

    bool Get(char *data, int &data_len, std::string &topic_name);

    inline unsigned long long GetMsgCount()
    {
        return msg_cnt_;
    }

private:
    bool LoadCfg(const char *cfg_file, RdKafka::Conf *conf, RdKafka::Conf *tconf, std::vector<std::string> &topics);

    bool msg_consume(RdKafka::Message *message, void *opaque);

private:
    std::vector<std::string> m_topics;
    RdKafka::KafkaConsumer *m_pConsumer;
    ExampleEventCb m_ex_event_cb;
    RdKafka::Message *m_kafka_msg;

    bool exit_eof_;
    int eof_cnt_;
    int partition_cnt_;
    unsigned long long msg_cnt_;
};

#endif