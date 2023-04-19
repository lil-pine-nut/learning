#include "mysqlclient.h"
#include <iostream>

MysqlClient::MysqlClient()
{
    m_client = NULL;
    m_client = mysql_init(NULL);
}

MysqlClient::~MysqlClient()
{
    mysql_close(m_client);
}

bool MysqlClient::isValid()
{
    return (m_client != NULL);
}

bool MysqlClient::connect(const std::string &host, const std::string &user, const std::string &password,
                          const std::string &database, unsigned short port)
{
    return mysql_real_connect(m_client, host.c_str(), user.c_str(), password.c_str(), database.c_str(), port, NULL, 0) != NULL;
}

bool MysqlClient::query(const std::string &sql)
{
    if (mysql_query(m_client, sql.c_str()))
    {
        std::cerr << "line: " << __LINE__ << ";" << mysql_errno(m_client) << ":" << mysql_error(m_client) << std::endl;
        return false;
    }
    return true;
}

bool MysqlClient::getData(const std::string &sql, std::vector<std::string> *fields, std::vector<std::vector<std::string> > *data)
{
    if (mysql_query(m_client, sql.c_str()))
    {
        std::cerr << "line: " << __LINE__ << ";" << mysql_errno(m_client) << ":" << mysql_error(m_client) << std::endl;
        return false;
    }

    // MYSQL_RES * result = mysql_store_result(m_client);//检索并存储整个结果集,即获得sql语句结束后返回的结果集
    MYSQL_RES *result = mysql_use_result(m_client); //开始逐行检索结果集。
    if (result != NULL)
    {

        unsigned int num_fields = mysql_num_fields(result); //每一行的字段数量
        my_ulonglong num_rows = mysql_num_rows(result);

        //容器预先分配空间
        fields->reserve(num_fields);
        data->reserve(num_rows);
        for (my_ulonglong i = 0; i < num_rows; i++)
        {
            data->at(i).reserve(num_fields);
        }

        MYSQL_FIELD *field = NULL;
        while (field = mysql_fetch_field(result)) //下一个表字段的类型
        {
            fields->push_back(field->name);
        }

        MYSQL_ROW row = NULL;
        while (row = mysql_fetch_row(result)) //	获取下一个结果集行
        {
            std::vector<std::string> linedata;
            for (unsigned int i = 0; i < num_fields; ++i)
            {
                linedata.push_back(row[i]);
            }
            data->push_back(linedata);
        }
    }
    else
    {
        std::cerr << "line: " << __LINE__ << ";" << mysql_errno(m_client) << ":" << mysql_error(m_client) << "; result is empty." << std::endl;
    }
    mysql_free_result(result);

    return true;
}