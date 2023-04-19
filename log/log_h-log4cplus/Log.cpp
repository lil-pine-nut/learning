#include "Log.h"
#include <iostream>

#ifdef USE_LOG4CPLUS
#include <log4cplus/configurator.h>
#include <log4cplus/appender.h>
#include <log4cplus/tstring.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/helpers/loglog.h>
#include <unistd.h>
#include <string.h>

log4cplus::Logger *g_logger = NULL;
bool InitLogger(string ConfigureFile, string logdir, string level)
{
    if (logdir.empty())
        logdir = "./logs";
    if (level.empty())
        level = "DEBUG";

    try
    {
        static log4cplus::Logger logMgr;
        if (!ConfigureFile.empty())
        {
            log4cplus::PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT(ConfigureFile.c_str()));
        }
        else
        {
            char name_buf[1024];
            sprintf(name_buf, "%d", getpid());
            string binary_name = name_buf;
            string tmp_file = "./" + binary_name + "_log4cplus.css.tmp";
            fstream fo(tmp_file.c_str(), ios::out);
            if (fo.good())
            {
                string defult_configer;
                defult_configer += "\nlog4cplus.rootLogger=" + level + ", LogFile, Console";
                defult_configer += "\nlog4cplus.appender.LogFile=log4cplus::TimeBasedRollingFileAppender";
                defult_configer += "\nlog4cplus.appender.LogFile.FilenamePattern=" + logdir + "/log-%d{yyyy-MM-dd}.log";
                defult_configer += "\nlog4cplus.appender.LogFile.Append=true";
                defult_configer += "\nlog4cplus.appender.LogFile.MaxHistory=999";
                defult_configer += "\nlog4cplus.appender.LogFile.RollOnClose=false";
                defult_configer += "\nlog4cplus.appender.LogFile.CreateDirs=true";
                defult_configer += "\nlog4cplus.appender.LogFile.layout=log4cplus::PatternLayout";
                defult_configer += "\nlog4cplus.appender.LogFile.layout.ConversionPattern=%D{%Y-%m-%d %H:%M:%S.%q} %-5p [%M] - %m %n";

                defult_configer += "\nlog4cplus.appender.Console=log4cplus::ConsoleAppender";
                defult_configer += "\nlog4cplus.appender.Console.Target=System.out";
                defult_configer += "\nlog4cplus.appender.Console.Threshold=ALL";
                defult_configer += "\nlog4cplus.appender.Console.layout=log4cplus::PatternLayout";
                defult_configer += "\nlog4cplus.appender.Console.layout.ConversionPattern=%D{%Y-%m-%d %H:%M:%S.%q} %-5p [%M] - %m %n";

                // cerr << defult_configer << endl;
                fo.write(defult_configer.c_str(), defult_configer.length());
                fo.close();

                log4cplus::PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT(tmp_file.c_str()));

                string cmd = "rm -f " + tmp_file;
                system(cmd.c_str());
            }
            else
            {
                fprintf(stderr, "set configure file(%s) failed!\n", tmp_file.c_str());
                log4cplus::BasicConfigurator config;
                config.configure();
            }
        }

        logMgr = log4cplus::Logger::getRoot();
        g_logger = &logMgr;

        // g_logger->setLogLevel();
    }
    catch (const std::exception &ex)
    {
        std::cout << "Log initialization failed: " << ex.what() << std::endl;
    }
}

void DropLogger()
{
    log4cplus::Logger::shutdown();
}
#endif

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