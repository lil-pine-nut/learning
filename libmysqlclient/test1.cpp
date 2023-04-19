#include "mysqlclient.h"
#include <iostream>
#include <time.h>

using namespace std;

#define MICRO_IN_SECOND 1000000
#define NANOS_IN_SECOND 1000000000

/**
 * @brief  获取当前毫秒数
 * 
 * @return double 返回当前的毫秒数
 */
double currentTimeInMiliSeconds() 
{
    struct timespec res;
    double ret = 0;
    clock_gettime(CLOCK_MONOTONIC, &res);
    ret = (double)(res.tv_sec * NANOS_IN_SECOND + res.tv_nsec) / MICRO_IN_SECOND;

    return ret;
}

int main()
{
    double start = currentTimeInMiliSeconds(); 
    try
    {
        MysqlClient mysqlclient;
        if(mysqlclient.isValid())
        {
            //连接接数据库
            if(!mysqlclient.connect("127.0.0.1", "test_user", "123456", "mydb_test"))
            {
                cerr << "mysqlclient connect failed." << endl;
                cerr << mysqlclient.erron() << ":" << mysqlclient.error() << endl; 
                return -1;
            }

            //创建数据表
            std::string sql =   "create table if not exists `students`(\
                                `id` INT auto_increment,\
                                `username` VARCHAR(128) ,\
                                `age` int,\
                                primary key(id))\
                                ENGINE=InnoDB DEFAULT CHARSET=utf8 ;";

            if(!mysqlclient.query(sql))
            {
                cerr << "mysqlclient create table failed." << endl;
            }

            //插入数据
            sql = "insert into students set username='张三', age=16;";
            if(!mysqlclient.query(sql))
            {
                cerr << "mysqlclient insert data failed." << endl;
            }
            sql = "insert into students set username='李四', age=15;";
            if(!mysqlclient.query(sql))
            {
                cerr << "mysqlclient insert data failed." << endl;
            }
            sql = "insert into students set username='王五', age=14;";
            if(!mysqlclient.query(sql))
            {
                cerr << "mysqlclient insert data failed." << endl;
            }
            sql = "insert into students set username='赵六', age=16;";
            if(!mysqlclient.query(sql))
            {
                cerr << "mysqlclient insert data failed." << endl;
            }

            //删除数据
            sql = "delete from students where id=4;";
            if(!mysqlclient.query(sql))
            {
                cerr << "mysqlclient delete data failed." << endl;
            }

            //更新数据
            sql = "update students set age=15 where username='zhangsan';"; 
            if(!mysqlclient.query(sql))
            {
                cerr << "mysqlclient update data failed." << endl;
            }

            //查询数据
            sql = "select * from students;";
            vector<string> fields_vec;
            std::vector<std::vector<std::string> > data_vec;
            if(!mysqlclient.getData(sql, &fields_vec, &data_vec))
            {
                cerr << "mysqlclient update data failed." << endl;
            }
            for(int i=0; i<fields_vec.size(); i++)
            {
                cout << fields_vec[i] << "\t";
            }
            cout << endl;

            for(int i=0; i<data_vec.size(); i++)
            {
                for(size_t j=0; j<data_vec[i].size(); j++)
                    cout << data_vec[i][j] << "\t";
                cout << endl;
            }

            //删除表
            sql = "drop table students;";
            if(!mysqlclient.query(sql))
            {
                cerr << "mysqlclient drop talbe failed." << endl;
            }
        }
        else
        {
            cerr << "mysqlclient is not Valid." << endl;
        }
    }
    catch(const std::exception& e)
    {
        cerr << e.what() << endl;
    }

    cout << "Cost time:" << currentTimeInMiliSeconds()-start << "ms" << endl;
    
}