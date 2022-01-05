#pragma once
#include <string>
#include <stdint.h>
#include <memory>
#include <list>
#include <sstream>
#include <fstream>
#include <vector>
#include <stdarg.h>
#include <map>
#include <iostream>
#include "../common/dy_singleton.hpp"

/**
 * @brief 日志模块。
 * @feature 同步; 输出至多文件; 流/格式化输出;
 * @todo todolist{;TODO:asyn};
 * @example 
 *      // 默认日志器
 *      dysv::infoStream << "some thing";   // 流式输出至控制台;
 *      dysv::info("welcome {}", "dysv");   // 格式化输出至控制台;
 *      dysv::set_level(dysv::level::info); // 设置默认日志器的日志级别，后续低于该级别的日志则不会落地;
 *      dysv::set_logger(logger);           // 更改默认的日志器;
 *      dysv::set_pattern("[%H:%M:%S %z] [%^%L%$] [thread %t] %v");   // 设置默认日志事件器。允许自行调整日志器格式;
 *      
 *      // 自定义的日志器
 *      auto file_logger = dysv::Logger("file_path", level = info, pattern = default);
 *      file_logger->trace("hello {}", "dy");   // not written, because default level is info.
 *      file_logger->warn("sad emerging...");   // written.
 * 
 */

namespace dysv
{
    /**
     * @brief 日志级别
     * 
     */
    namespace level
    {
        enum LevelEnum
        {
            TRACE = 0,
            INFO,
            WARN,
            ERROR,
            FATAL,
            UNKNOW
        };
        const std::string to_string(level::LevelEnum lv);
        level::LevelEnum to_enum(const std::string &lv);
    } // namespace level

    /**
     * @brief 日志器。通过level校验，将各个日志消息format后，再经过日志事件构造pattern, 通过sink落到目的地。
     * 
     */
    class Logger{
    public:
        using ptr = std::shared_ptr<dysv::Logger>;
    };

    /**
     * @brief 日志事件。将日志标准化，便于从日志文件中分析每条日志[when][who][where]的信息，防止日志文件杂糅在一起。
     * 
     */
    class LoggerEvent;
    /**
     * @brief 日志输出器。将日志真正落地。
     * 
     */
    class LoggerSink;
    /**
     * @brief 日志格式化器。允许用户使用类似于printf中占位符的方式格式化输出。
     * 
     */
    class LoggerFormatter;
    /**
     * @brief 管理所有的日志器。
     * 
     */
    class LoggerManger{
    public:
        /**
         * @brief Construct a new Logger Manger object
         * 
         */
        LoggerManger();
        
        /**
         * @brief init the LoggerManger.
         * 
         */
        void Init();
        
        
        bool AddLogger();
        
        
        // Logger::ptr GetLogger(const std::string& name);
        
        // Logger::ptr GetDefault() const{return m_default_logger;} 
    private:
        // TODO:lock
        Logger::ptr m_default_logger;
        std::map<std::string, Logger::ptr> m_loggers;
    };
    
    // using LoggerMgr = dysv::Singleton<LoggerManger>;

    // default log function.
    void trace(const std::string& str);
    void info(const std::string& str);
    void warn(const std::string& str);
    void error(const std::string& str);
    void fatal(const std::string& str);

    void set_level(level::LevelEnum lv);
    void set_logger(Logger lg);
    void set_pattern(const std::string& str);

} // namespace dysv