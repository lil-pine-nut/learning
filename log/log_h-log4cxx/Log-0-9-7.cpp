#include "Log-0-9-7.h"
#include <iostream>

#ifdef USE_LOG4CXX_0_9_7
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include <unistd.h>
#include <string.h>
#include <fstream>
#include <stdlib.h>

log4cxx::LoggerPtr *g_logger = NULL;
bool InitLogger(string ConfigureFile, string logdir, string level)
{
    if (logdir.empty())
        logdir = "./logs";
    if (level.empty())
        level = "DEBUG";

    try
    {
        if (!ConfigureFile.empty())
        {
            log4cxx::PropertyConfigurator::configure((ConfigureFile.c_str()));
        }
        else
        {
            char name_buf[1024];
            sprintf(name_buf, "%d", getpid());
            string binary_name = name_buf;
            string tmp_file = "./" + binary_name + "_log4cxx.css.tmp";
            fstream fo(tmp_file.c_str(), ios::out);
            if (fo.good())
            {
                string defult_configer;
                defult_configer += "\nlog4j.rootLogger=" + level + ", LogFile, Console";
                defult_configer += "\nlog4j.appender.LogFile=org.apache.log4j.DailyRollingFileAppender";
                defult_configer += "\nlog4j.appender.LogFile.File=" + logdir + "/log.log";
                defult_configer += "\nlog4j.appender.LogFile.DataPattern='.'yyyy-MM-dd";
                defult_configer += "\nlog4j.appender.LogFile.Append=true";
                defult_configer += "\nlog4j.appender.LogFile.ImmediateFlush=true";
                defult_configer += "\nlog4j.appender.LogFile.layout=org.apache.log4j.PatternLayout";
                defult_configer += "\nlog4j.appender.LogFile.layout.ConversionPattern=%d{%Y-%m-%d %H:%M:%S} [%p] %c [%F] - %m%n";

                defult_configer += "\nlog4j.appender.Console=org.apache.log4j.ConsoleAppender";
                defult_configer += "\nlog4j.appender.Console.Target=System.out";
                defult_configer += "\nlog4j.appender.Console.layout=org.apache.log4j.PatternLayout";
                defult_configer += "\nlog4j.appender.Console.layout.ConversionPattern=%d{%Y-%m-%d %H:%M:%S} %-5p [%F] - %m%n";

                // cerr << defult_configer << endl;
                fo.write(defult_configer.c_str(), defult_configer.length());
                fo.close();

                log4cxx::PropertyConfigurator::configure((tmp_file.c_str()));

                string cmd = "rm -f " + tmp_file;
                system(cmd.c_str());
            }
            else
            {
                fprintf(stderr, "set configure file(%s) failed!\n", tmp_file.c_str());
                log4cxx::BasicConfigurator::configure();
            }
        }
        static log4cxx::LoggerPtr logMgr = log4cxx::Logger::getRootLogger();
        g_logger = &logMgr;
    }
    catch (const std::exception &ex)
    {
        std::cout << "Log initialization failed: " << ex.what() << std::endl;
    }
}

inline const char *strrnchr(const char *const str, const char *const str_end, const char ch)
{
    for (const char *ptr = str_end - 1; ptr >= str; --ptr)
    {
        if (*ptr == ch)
        {
            return ptr;
        }
    }
    return NULL;
}

/**
 * @brief 获取类名，当仅为非类的成员函数时，获取函数名
 *
 * @param pPrettyFunc
 * @return string
 */
string getClassName(const char *pPrettyFunc)
{
    const char *pRight = index(pPrettyFunc, ':');
    if (NULL == pRight)
    {
        pRight = index(pPrettyFunc, '(');
        if (NULL == pRight)
            return pPrettyFunc;
    }
    const char *pLeft = strrnchr(pPrettyFunc, pRight, ' ');
    if (NULL == pLeft)
        return string(pPrettyFunc, pRight - pPrettyFunc);

    return string(pLeft + 1, pRight - pLeft - 1);
}

#endif
