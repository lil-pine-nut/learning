#include "KafkaConsumer.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>

using namespace std;

KafkaConsumer::KafkaConsumer(/* args */)
{
    m_pConsumer = NULL;

    exit_eof_ = true;
    eof_cnt_ = 0;
    partition_cnt_ = 0;
    msg_cnt_ = 0;

    m_kafka_msg = NULL;
}

KafkaConsumer::~KafkaConsumer()
{
    if (m_pConsumer)
    {
        m_pConsumer->close();
        delete m_pConsumer;
    }
    //应用退出之前等待rdkafka清理资源
    RdKafka::wait_destroyed(5000);
}

bool KafkaConsumer::Init(std::string configFile)
{
    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    RdKafka::Conf *tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

    if (!LoadCfg(configFile.c_str(), conf, tconf, m_topics))
    {
        delete conf;
        delete tconf;
        return false;
    }

    std::string errstr;

    if (conf->set("event_cb", &m_ex_event_cb, errstr) != RdKafka::Conf::CONF_OK)
        fprintf(stderr, "set event_cb ERR[%s]\n", errstr.c_str());

    if (conf->set("default_topic_conf", tconf, errstr) != RdKafka::Conf::CONF_OK)
        fprintf(stderr, "set default_topic_conf ERR[%s]\n", errstr.c_str());

    m_pConsumer = RdKafka::KafkaConsumer::create(conf, errstr);
    if (!m_pConsumer)
    {
        delete conf;
        delete tconf;
        return false;
    }

    RdKafka::ErrorCode err = m_pConsumer->subscribe(m_topics);
    if (err)
    {
        std::cerr << "Failed to subscribe to " << m_topics.size() << " topics: "
                  << RdKafka::err2str(err) << std::endl;
        delete conf;
        delete tconf;
        return false;
    }
    return true;
}

bool KafkaConsumer::Get(char *data, int &data_len, std::string &topic_name)
{
    // 1000毫秒未订阅到消息，触发RdKafka::ERR__TIMED_OUT
    RdKafka::Message *msg = m_pConsumer->consume(1000);
    bool ret = msg_consume(msg, NULL);
    if (ret)
    {
        if (m_kafka_msg)
            delete m_kafka_msg;
        m_kafka_msg = msg;
        data = (char *)m_kafka_msg->payload();
        data_len = m_kafka_msg->len();
        topic_name = m_kafka_msg->topic_name();
    }
    else
    {
        data = NULL;
        data_len = 0;
        delete msg;
    }

    return ret;
}

bool KafkaConsumer::LoadCfg(const char *cfg_file, RdKafka::Conf *conf, RdKafka::Conf *tconf, std::vector<std::string> &topics)
{
    ifstream ifs(cfg_file);
    if (!ifs)
    {
        cerr << "examples:\n"
                "################### CFG EXAMPLES #####################\n"
                "[CONFIG]\n"
                "metadata.broker.list=127.0.0.1:9092\n"
                "#####group.id必须设置#####\n"
                "group.id=test_2022_05_24\n"
                "\n"
                "##### PLAIN 使用账号密码 ######\n"
                "#security.protocol=SASL_PLAINTEXT\n"
                "#sasl.mechanisms=PLAIN\n"
                "#sasl.username=my_test_name\n"
                "#sasl.password=my_test_passwd\n"
                "\n"
                "##### PLAIN - 使用keytab ######\n"
                "#security.protocol=SASL_PLAINTEXT\n"
                "#sasl.kerberos.keytab=/var/kerberos/krb5kdc/kadm5.keytab\n"
                "#sasl.kerberos.principal=kafka/centos7-6@HADOOP.COM\n"
                "\n"
                "#enable.partition.eof=true\n"
                "#api.version.request=true\n"
                "#debug=all\n"
                "[TOPIC]\n"
                "##### 设置topic ######\n"
                "#topic=test\n"
                "[TOPIC-CONFIG]\n"
                "##### 设置消费offset ######\n"
                "#auto.offset.reset=latest\n"
                "###################################################\n"
             << endl;

        return false;
    }

    enum CONFIG_CFG
    {
        CONFIG,
        TOPIC,
        TOPIC_CONFIG,
        OTHER,
    };
    string errstr;
    char line[4096] = {0};
    int cfg_value = OTHER;
    while (ifs.getline(line, sizeof(line) - 1))
    {
        if (line[0] == '#')
            continue;
        if (strncmp(line, "[CONFIG]", strlen("[CONFIG]")) == 0)
        {
            cfg_value = CONFIG;
            continue;
        }
        else if (strncmp(line, "[TOPIC]", strlen("[TOPIC]")) == 0)
        {
            cfg_value = TOPIC;
            continue;
        }
        else if (strncmp(line, "[TOPIC-CONFIG]", strlen("[TOPIC-CONFIG]")) == 0)
        {
            cfg_value = TOPIC_CONFIG;
            continue;
        }

        char *pEQ = strchr(line, '=');
        if (pEQ == NULL)
            continue;

        pEQ[0] = '\0';
        pEQ++;
        switch (cfg_value)
        {
        case CONFIG:
            if (conf->set(line, pEQ, errstr) == RdKafka::Conf::CONF_OK)
                fprintf(stderr, "%s=%s\n", line, pEQ);
            else
                fprintf(stderr, "ERR[%s]:%s=%s\n", errstr.c_str(), line, pEQ);
            break;

        case TOPIC:
            topics.push_back(pEQ);
            break;

        case TOPIC_CONFIG:
            if (tconf->set(line, pEQ, errstr) == RdKafka::Conf::CONF_OK)
                fprintf(stderr, "%s=%s\n", line, pEQ);
            else
                fprintf(stderr, "ERR[%s]:%s=%s\n", errstr.c_str(), line, pEQ);
            break;

        default:
            break;
        }
    }
    return true;
}

bool KafkaConsumer::msg_consume(RdKafka::Message *message, void *opaque)
{
    switch (message->err())
    {
    case RdKafka::ERR__TIMED_OUT:
        // std::cerr << "RdKafka::ERR__TIMED_OUT"<<std::endl;
        break;

    case RdKafka::ERR_NO_ERROR:
        /* Real message */
        ++msg_cnt_;
        return true;
        break;

    case RdKafka::ERR__PARTITION_EOF:
        /* Last message */
        if (exit_eof_ && ++eof_cnt_ == partition_cnt_)
        {
            std::cerr << "%% EOF reached for all " << partition_cnt_ << " partition(s)" << std::endl;
        }
        break;

    case RdKafka::ERR__UNKNOWN_TOPIC:
    case RdKafka::ERR__UNKNOWN_PARTITION:
        std::cerr << "Consume failed: " << message->errstr() << std::endl;
        break;

    default:
        /* Errors */
        std::cerr << "Consume failed: " << message->errstr() << std::endl;
        break;
    }
    return false;
}
