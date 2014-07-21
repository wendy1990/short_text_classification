/** 
 * @File: log.hpp
 * @brief: log基本设置,日志系统采用log4cplus
 * @Author: xuqiang
 *
 */

#ifndef __LOG_HPP__
#define __LOG_HPP__

#include <string>
#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/stringhelper.h>
#include <iostream>
#include <boost/thread/detail/singleton.hpp>

/**
 * @brief log4cplugs的封装
 *  usage：
 *      SingletonLogger::instance().initialize("log4cplugs.propertes");
 *      // that's OK, now use macro write log
 *      LOG_ERROR("Init kafka broker list: " << serverAddrStr);
 */
class log_wrapper 
{
public:
    bool initialize(const std::string config)
    {
        mLogger = log4cplus::Logger::getRoot();
        try{
            log4cplus::PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT(config.c_str()));
            mLogger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("filelogger"));
        } catch(...) {
            std::cerr << "initialize log4cplus failed!" << std::endl;
            return false;
        }
        return true;
    }
    
    inline const log4cplus::Logger& get_logger() const
    {
        return mLogger;
    }
    
private:
    log4cplus::Logger   mLogger;
};

typedef boost::detail::thread::singleton<log_wrapper> SingletonLogger;

#define LOG_FATAL(fields) LOG4CPLUS_FATAL(SingletonLogger::instance().get_logger(),fields)
#define LOG_ERROR(fields) LOG4CPLUS_ERROR(SingletonLogger::instance().get_logger(),fields)
#define LOG_WARN(fields) LOG4CPLUS_WARN(SingletonLogger::instance().get_logger(),fields)
#define LOG_INFO(fields) LOG4CPLUS_INFO(SingletonLogger::instance().get_logger(),fields)
#define LOG_DEBUG(fields) LOG4CPLUS_DEBUG(SingletonLogger::instance().get_logger(),fields)
#define LOG_TRACE(fields) LOG4CPLUS_TRACE(SingletonLogger::instance().get_logger(),fields)

#endif    /* __LOG_HPP__ */

