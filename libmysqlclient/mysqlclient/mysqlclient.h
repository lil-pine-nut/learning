#ifndef __MYSQL_CLIENT_CLASS__H
#define __MYSQL_CLIENT_CLASS__H

#include "mysql.h"
#include <string>
#include <vector>

class MysqlClient
{
public:
    MysqlClient();
    ~MysqlClient();

    /**
     * @brief 改类是否有效，即m_client初始化后是否为空
     *
     * @return true
     * @return false
     */
    bool isValid();

    /**
     * @brief 连接
     *
     * @param host      ip地址host
     * @param user      用户名
     * @param password  密码
     * @param database  数据库
     * @param port      端口
     * @return true
     * @return false
     */
    bool connect(const std::string &host, const std::string &user, const std::string &password,
                 const std::string &database = "", unsigned short port = 3306);

    /**
     * @brief 执行sql语法
     *
     * @param sql  语法：
     * @param 创建数据库    std::string sql = "create database if not exists test_db;";
     * @param 进入数据库    std::string sql = "use test_db;";
     * @param 创建表        std::string sql =   "create table if not exists 'students'(\
                                                'id' INT auto_increment,\
                                                'username' VARCHAR(128),\
                                                'age' int,\
                                                primary key(id))\
                                                default charset = utf8;";
     * @param 删除表        std::string sql = "drop table students;";
     * @param 更新数据      std::string sql = "update students set age=15 where username='zhangsan';";
     * @param 删除数据      std::string sql = "delete from students where id=12;";
     *
     * @param 插入数据      std::string sql =   "insert into students('username','age')\
                                                values ('lisi', 16);";
                或者：      std::string sql = "insert into students set username='lisi', age=16;";
     * @param 查询数据      std::string sql = "select * from students;";//查询数据后,使用getResult()获取MYSQL_RES指针自己操作。或者使用getData()函数。

     * @return true
     * @return false
     */
    bool query(const std::string &sql);

    /**
     * @brief 查询数据
     *
     * @param sql       查询数据语法，e.g: std::string sql = "select * from students;";
     * @param fields    返回的字段名称
     * @param data      返回查询的所有数据：一行行的数据，data.size()为行数，data[0].size()为字段数量。
     * @return true
     * @return false
     */
    bool getData(const std::string &sql, std::vector<std::string> *fields, std::vector<std::vector<std::string> > *data);

    std::string error() const
    {
        return mysql_error(m_client);
    }

    unsigned int erron() const
    {
        return mysql_errno(m_client);
    }

    unsigned long long affected_rows() const
    {
        return mysql_affected_rows(m_client); //行数改变/删除/最后插入
    }

    unsigned long long inserted_id() const
    {
        return mysql_insert_id(m_client); //前面的语句为AUTO_INCREMENT列生成的ID
    }

    MYSQL_RES *getResult() const
    {
        return mysql_use_result(m_client); //开始逐行检索结果集
    }

private:
    /* data */
    MYSQL *m_client;
};

#endif