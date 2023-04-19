#include <iostream>
#include <stdio.h>
#include "mysql.h"

using namespace std;

int main()
{
    MYSQL mysql;
    try
    {
        //初始化mysql
        mysql_init(&mysql); //连接mysql，数据库
        const char host[] = "localhost";
        const char user[] = "test_user";
        const char psw[] = "123456";
        const char db_name[] = "mydb_test";
        const int port = 3306;
        //返回false则连接失败，返回true则连接成功
        if (!(mysql_real_connect(&mysql, host, user, psw, db_name, port, NULL, 0)))
        //中间分别是主机，用户名，密码，数据库名，端口号（可以写默认0或者3306等），可以先写成参数再传进去
        {
            printf("Error connecting to database:%s\n", mysql_error(&mysql));
            return false;
        }
        else
        {
            printf("Connected...\n");
            // return true;
        }

        std::string queryStr = "use mydb_test";
        if (0 == mysql_query(&mysql, queryStr.c_str()))
        {
            printf("use mydb_test success ...\n");
            // return true;
        }
        else
        {
            printf("Error connecting to database:%s\n", mysql_error(&mysql));
        }

        std::string dbname = "students";
        queryStr = "CREATE TABLE IF NOT EXISTS ";
        queryStr += dbname;
        std::string table_property = "(id INT AUTO_INCREMENT PRIMARY KEY,username VARCHAR(255),age INT) AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;";
        queryStr += table_property;
        // cout << queryStr << endl;
        if (mysql_query(&mysql, queryStr.c_str()))
        {
            printf("Error CREATE TABLE:%s\n", mysql_error(&mysql));
            return false;
        }

        //插入数据,事务
        std::string sql = "begin;";
        mysql_query(&mysql, sql.c_str());

        sql = "insert into students(username,age)\
                values (\"zhangsan\",\"12\");";
        if (mysql_query(&mysql, sql.c_str()))
        {
            cout << "line: " << __LINE__ << ";" << mysql_error(&mysql) << mysql_errno(&mysql) << endl;
        }

        sql = "commit;";
        mysql_query(&mysql, sql.c_str());
        //更新数据
        sql = "update students set username = 'huang' where id = 1;";
        if (mysql_query(&mysql, sql.c_str()))
        {
            cout << "line: " << __LINE__ << ";" << mysql_error(&mysql) << mysql_errno(&mysql) << endl;
        }

        //查询数据
        sql = "select * from students;";
        if (mysql_query(&mysql, sql.c_str()))
        {
            cout << "line: " << __LINE__ << ";" << mysql_error(&mysql) << mysql_errno(&mysql) << endl;
            throw -1;
        }
        else
        {
            //读取检索的结果
            MYSQL_RES *result = mysql_use_result(&mysql);
            if (result != NULL)
            {
                MYSQL_ROW row;
                int num_fields = mysql_num_fields(result); //每一行的字段数量
                while (row = mysql_fetch_row(result))
                {
                    if (row == NULL)
                    {
                        break;
                    }
                    else
                    {
                        for (int i = 0; i < num_fields; ++i)
                        {
                            cout << row[i] << " ";
                        }
                        cout << endl;
                    }
                }
            }
            mysql_free_result(result);
        }
    }
    catch (...)
    {
        cout << "MySQL operation is error!" << endl;
    }

    mysql_close(&mysql);
    return 0;
}