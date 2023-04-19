#include "KafkaProducer.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>
using namespace std;

KafkaProducer::KafkaProducer(/* args */)
{
    m_pProducer = NULL;
}

KafkaProducer::~KafkaProducer()
{
    TopicMap::iterator it = m_TopicMap.begin();
    while (it != m_TopicMap.end())
    {
        delete it->second;
        m_TopicMap.erase(it++);
    }
    if (m_pProducer)
        delete m_pProducer;

    //应用退出之前等待rdkafka清理资源
    RdKafka::wait_destroyed(5000);
}

bool KafkaProducer::Init(std::string configFile)
{
    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    RdKafka::Conf *tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

    std::vector<std::string> topics;

    if (!LoadCfg(configFile.c_str(), conf, tconf, topics))
    {
        delete conf;
        delete tconf;
        return false;
    }

    string errstr;
    if (conf->set("event_cb", &m_ex_event_cb, errstr) != RdKafka::Conf::CONF_OK)
        fprintf(stderr, "set event_cb ERR[%s]\n", errstr.c_str());

    // 会打印很多信息，不用
    // if(conf->set("dr_cb", &m_ex_dr_cb, errstr) != RdKafka::Conf::CONF_OK)
    //     fprintf(stderr, "set dr_cb ERR[%s]\n", errstr.c_str());

    m_pProducer = RdKafka::Producer::create(conf, errstr);
    if (!m_pProducer)
    {
        std::cerr << "Failed to create producer: " << errstr << std::endl;
        delete conf;
        delete tconf;
        return false;
    }

    if (topics.empty())
    {
        std::cerr << "Not Set The Topic !!!" << std::endl;
        delete conf;
        delete tconf;
        return false;
    }

    for (int i = 0; i < topics.size(); i++)
    {
        RdKafka::Topic *topic = RdKafka::Topic::create(m_pProducer, topics[i],
                                                       tconf, errstr);
        if (topic)
            m_TopicMap.insert(make_pair(topics[i], topic));
        else
            std::cerr << "Failed to create topic: " << errstr << std::endl;
    }
    delete conf;
    delete tconf;
    return true;
}

void KafkaProducer::Send(const char *data, size_t data_len)
{
    for (TopicMap::iterator it = m_TopicMap.begin(); it != m_TopicMap.end(); ++it)
    {
        // cerr << it->first << ", " << it->second->name()<< endl;
        RdKafka::ErrorCode resp =
            m_pProducer->produce(it->second, RdKafka::Topic::PARTITION_UA,
                                 RdKafka::Producer::RK_MSG_COPY /* Copy payload */,
                                 (char *)data, data_len,
                                 NULL, NULL);
        if (resp != RdKafka::ERR_NO_ERROR)
            std::cerr << "% Produce failed: " << RdKafka::err2str(resp) << std::endl;

        m_pProducer->poll(0);
    }
}

bool KafkaProducer::LoadCfg(const char *cfg_file, RdKafka::Conf *conf, RdKafka::Conf *tconf, std::vector<std::string> &topics)
{
    ifstream ifs(cfg_file);
    if (!ifs)
    {
        cerr << "examples:\n"
                "################### CFG EXAMPLES #####################\n"
                "[CONFIG]\n"
                "metadata.broker.list=127.0.0.1:9092\n"
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