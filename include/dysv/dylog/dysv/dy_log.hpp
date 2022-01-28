#pragma once
#include <string>
#include <stdio.h>
#include <stdint.h>
#include <memory>
#include <list>
#include <sstream>
#include <fstream>
#include <vector>
#include <stdarg.h>
#include <map>
#include <iostream>
#include <unistd.h>
#include <sys/syscall.h>
#include <algorithm>
#include <thread>
#include "../common/dy_singleton.hpp"

/**
 * @brief 日志模块。
 * @feature 同步; 输出至多文件; 流/格式化输出;
 * @todo asyn; mutex; sink cache; exception;
 * @example 
 *      // 默认日志器
 *      dysv::infoStream << "some thing";   // 流式输出至控制台;    //todo
 *      dysv::info("welcome {}", "dysv");   // 格式化输出至控制台;  //todo
 *      dysv::set_level(dysv::level::info); // 设置默认日志器的日志级别，后续低于该级别的日志则不会落地;
 *      dysv::set_logger(logger);           // 更改默认的日志器;
 *      dysv::set_pattern("[%D %H:%M:%S:%s][%T][%F][%L][%P][%C]");   // 设置默认日志模式;
 *      
 *      // 自定义的日志器
 *      auto file_logger = dysv::Logger("name", "file_path", level = info, pattern = default);
 *      file_logger->trace("hello {}", "dy");   // not written, because default level is info.
 *      file_logger->warn("sad emerging...");   // written.
 * 
 */

namespace dysv
{
    // log with format
#define DEFAULT_LOGGER_NAME     "__root__"
#define STD_COUT                "__stdout__"
#define DEFAULT_LOGGER          (dysv::LoggerMgr::GetInstance()->GetDefaultLog())
#define DY_LOG_LEVEL(lv, txt, ...)   (DEFAULT_LOGGER->Log(std::make_shared<LogAdditionInfo>(__FILE__, __LINE__), lv, txt, __VA_ARGS__))
#define DY_LOG_TRACE(txt,...)       (DY_LOG_LEVEL(dysv::level::TRACE, txt, __VA_ARGS__))
#define DY_LOG_INFO(txt,...)        (DY_LOG_LEVEL(dysv::level::INFO,  txt, __VA_ARGS__))
#define DY_LOG_WARN(txt,...)        (DY_LOG_LEVEL(dysv::level::WARN,  txt, __VA_ARGS__))
#define DY_LOG_ERROR(txt,...)       (DY_LOG_LEVEL(dysv::level::ERROR, txt, __VA_ARGS__))
#define DY_LOG_FATAL(txt,...)       (DY_LOG_LEVEL(dysv::level::FATAL, txt, __VA_ARGS__))
    // 以下宏功能与default log function一致，但是为了样式统一也提供了宏
#define SET_LEVEL(lv)           (DEFAULT_LOGGER->SetLevel(lv))
#define SET_LOGGER(lg)          (DEFAULT_LOGGER->SetLogger(lg))
#define SET_PATTERN(str)        (DEFAULT_LOGGER->SetPattern(str))
#define ADD_SINK(sk)            (DEFAULT_LOGGER->AddSink(sk))
#define CLEAN_SINK()            (DEFAULT_LOGGER->CleanSink())


#define DEFAULT_PATTERN_STR "[%D %H:%M:%S:%s][%T][%F][%L][%P][%C]"

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
     * @brief 占位符。用于自定义日志模式。
     * 
     */
    namespace placeholder{
        /**
         * @brief 占位符枚举类型
         * 
         */
        enum PlaceholderType{
            n_NEW_LINE = 0,     // n, 换行符
            t_TAB,              // t, 制表符
            T_THREAD_ID,        // T, 线程ID
            F_FILE_NAME,        // F, 打印日志时的文件名
            L_LINE,             // L, 打印日志时的行号
            P_PRIORITY,         // P, 日志级别
            C_CONTENT,          // C, 日志内容
            D_DATE,             // D, 日期。eg: 2022/1/1
            H_HOUR,             // H, 小时。eg: 08
            M_MINUTE,           // M, 分钟。eg: 59
            S_SECOND,           // S, 秒。eg: 33
            s_MILLISECOND,      // s, 毫秒。eg: 012
            MAX_PATTERN         // 未知标识符，以' '替代
        };
        /**
         * @brief 占位符转为char类型。eg. to_char(T_THREAD_ID) == 'T'
         * 
         * @param pt 占位符枚举类型
         * @return char 转换结果
         */
        char to_char(PlaceholderType pt);

        /**
         * @brief char转回枚举类型。eg. to_enum('S') == S_SECOND
         * 
         * @param pt 待转换字符
         * @return PlaceholderType 转换后枚举类型
         */
        PlaceholderType to_enum(const char& pt);
    } // namespace placeholder

    class LogAdditionInfo;
    class Logger;
    class LoggerPattern;
    class LoggerSinkInterface;
    class LoggerManger;
    /**
     * @brief 除日志内容与日志级别，为LogPattern格式化提供额外的辅助信息。
     * 
     */
    class LogAdditionInfo{
    public:
        using ptr = std::shared_ptr<LogAdditionInfo>;
        // 文件名和行号必须在调用处传入
        LogAdditionInfo(const std::string& file, uint64_t line);
        std::string GetFileName() const;
        std::string GetLineNumber() const;
        std::string GetThreadId() const;
        std::string GetDate() const;
        std::string GetHours() const;
        std::string GetMinutes() const;
        std::string GetSeconds() const;
        std::string GetMilliseconds() const;
        std::string GetAdditionInfoByPlaceholder(char plchld) const;
        std::string GetAdditionInfoByPlaceholder(placeholder::PlaceholderType plchld) const;
    private:
        std::string         m_file_name; // 记录日志处所在文件名
        uint64_t            m_line_num;  // 记录日志处所在文件行号
        std::thread::id     m_thread_id; // 记录日志的线程ID
        uint64_t            m_time;      // 记录日志的时间(TODO boost::posix_time)
        uint64_t            m_elapse;    // 从线程启动到日志记录时已流逝的时间
    };

    /**
     * @brief 日志事件。将日志标准化，便于从日志文件中分析每条日志[when][who][where]的信息，防止日志文件杂糅在一起。
     * 
     */
    class LoggerPattern
    {
    public:
        using ptr = std::shared_ptr<LoggerPattern>;
        LoggerPattern();
        
        // 格式化+模式化
        std::string FmtAndPatternLog(LogAdditionInfo::ptr other_info, 
                                        level::LevelEnum lv, 
                                        const std::string& org_str, 
                                        va_list strArgs);
        
        // 模式化
        std::string PatternLog(LogAdditionInfo::ptr other_info, 
                                level::LevelEnum lv, 
                                const std::string &content);
        
        /**
         * @brief 格式化日志串
         * 
         */
        static std::string FormatLog(const std::string &content, ...);
        static std::string FormatLog(const std::string &content, va_list args);
        
        /**
         * @brief Get the Default Pattern Str object
         * 
         * @return std::string 默认模式串
         */
        static std::string GetDefaultPatternStr();
        void SetPatternStr(const std::string & str);
        std::string GetPatternStr();
        void Reset2Default();
    private:
        std::string m_pattern_str;
    };

    /**
     * @brief 日志输出器接口
     * 
     */
    class LoggerSinkInterface
    {
    public:
        using ptr = std::shared_ptr<LoggerSinkInterface>;
        virtual ~LoggerSinkInterface();
        virtual bool Sink(std::string content) = 0;
        std::string GetName();
    private:
        std::string m_name;
    };

    /**
     * @brief 日志器。通过level校验，将各个日志消息format后，再经过日志事件构造pattern, 通过sink落到目的地。
     * 
     */
    class Logger
    {
    public:
        using ptr = std::shared_ptr<dysv::Logger>;
        /// 构造函数、拷贝构造、赋值构造
        Logger(const std::string &name);
        // Logger(const Logger &lg) = delete;
        // Logger &operator=(const Logger &lg) = delete;

        /// 落日志
        void Log(LogAdditionInfo::ptr addtion_info, level::LevelEnum lv, const std::string& str);

        /// 辅助函数
        void Reset();
        const std::string GetName();

        /// 日志模式相关
        LoggerPattern::ptr GetPattern();
        void SetPattern(const std::string &placeholder);
        void SetPattern(LoggerPattern::ptr placeholder);

        /// 日志级别相关
        level::LevelEnum getLevel();
        void SetLevel(level::LevelEnum lv);
        void SetLevel(const std::string &lv);
        level::LevelEnum GetLevel();

        /// 日志sink相关
        LoggerSinkInterface::ptr GetLoggerSink(const std::string &name);
        void AddSink(LoggerSinkInterface::ptr sink);
        void DelSink(LoggerSinkInterface::ptr sink);
        void CleanSink();
    private:
        std::string m_name;
        level::LevelEnum m_level;
        std::map<std::string, LoggerSinkInterface::ptr> m_sinks;
        LoggerPattern::ptr m_pattern;
    };



    /**
     * @brief 管理所有的日志器。
     * 
     */
    class LoggerManger
    {
    public:
        LoggerManger();

        dysv::Logger::ptr GetDefaultLog();

        bool AddLogger(const std::string &name);

        Logger::ptr GetLogger(const std::string& name);
    private:
        // TODO:lock
        Logger::ptr m_default_logger;
        std::map<std::string, Logger::ptr> m_loggers;
    };


    using LoggerMgr = dysv::Singleton<LoggerManger>;

    // default log function.
    void trace(const std::string &str);
    void info(const std::string &str);
    void warn(const std::string &str);
    void error(const std::string &str);
    void fatal(const std::string &str);

    void set_level(level::LevelEnum lv);
    void set_logger(Logger lg);
    void set_pattern(const std::string &str);
    void add_sink(LoggerSinkInterface::ptr sink);
    void clean_sink();

} // namespace dysv