#include "dysv/dy_log.hpp"

namespace dysv{
    #define FOMATE_STR_BUFFER_SIZE  4096
    #define MICROSECONDS_PER_MILLISECONDS 1000

    /*********************namespace level**************************************/
    namespace level{
        const std::string to_string(level::LevelEnum lv){
            switch(lv){
                case TRACE:
                    return "TRACE";
                case INFO:
                    return "INFO";
                case WARN:
                    return "WARN";
                case ERROR:
                    return "ERROR";
                case FATAL:
                    return "FATAL";
                default:
                    return "UNKNOW";
            }
            return "UNKNOW";
        }

        level::LevelEnum to_enum(const std::string &lv){
            if (lv == "TRACE" || lv == "trace"){
                return level::TRACE;
            }
            if(lv == "INFO" || lv == "info"){
                return level::INFO;
            }
            if(lv == "WARN" || lv == "warn"){
                return level::WARN;
            }
            if(lv == "ERROR" || lv == "error"){
                return level::ERROR;
            }
            if(lv == "FATAL" || lv == "fatal"){
                return level::FATAL;
            }
            return level::UNKNOW;
        }
    } // end of namespace level

    /*********************namespace placeholder********************************/
    namespace placeholder{
        char to_char(PlaceholderType pt){
            switch(pt){
                case placeholder::n_NEW_LINE:
                    return '\n';
                case placeholder::t_TAB:
                    return '\t';
                case placeholder::T_THREAD_ID:
                    return 'T';
                case placeholder::F_FILE_NAME:
                    return 'F';
                case placeholder::L_LINE:
                    return 'L';
                case placeholder::P_PRIORITY:
                    return 'P';
                case placeholder::C_CONTENT:
                    return 'C';
                case placeholder::D_DATE:
                    return 'D';
                case placeholder::H_HOUR:
                    return 'H';
                case placeholder::M_MINUTE:
                    return 'M';
                case placeholder::S_SECOND:
                    return 'S';
                case placeholder::s_MILLISECOND:
                    return 's';
                default:
                    return ' ';
            }
            return ' ';
        }

        PlaceholderType to_enum(const char &pt){
            switch(pt){
                case 'n':
                    return placeholder::n_NEW_LINE;
                case 't':
                    return placeholder::t_TAB;
                case 'T':
                    return placeholder::T_THREAD_ID;
                case 'F':
                    return placeholder::F_FILE_NAME;
                case 'L':
                    return placeholder::L_LINE;
                case 'P':
                    return placeholder::P_PRIORITY;
                case 'C':
                    return placeholder::C_CONTENT;
                case 'D':
                    return placeholder::D_DATE;
                case 'H':
                    return placeholder::H_HOUR;
                case 'M':
                    return placeholder::M_MINUTE;
                case 'S':
                    return placeholder::S_SECOND;
                case 's':
                    return placeholder::s_MILLISECOND;
                default:
                    return placeholder::MAX_PATTERN;
            }
            return placeholder::MAX_PATTERN;
        }
    } // end of namespace placeholder

    /*******************class LogAdditionInfo***********************************/
    LogAdditionInfo::LogAdditionInfo(const std::string& file, uint64_t line)
                                    : m_file_name(file), m_line_num(line){
        m_thread_id = syscall(SYS_gettid);
        timespec_get(&m_time, TIME_UTC);

        // 格式化秒级时间
        m_tm = localtime(&m_time.tv_sec);

        m_elapse = clock();
    }
    std::string LogAdditionInfo::GetFileName() const {return m_file_name;}
    std::string LogAdditionInfo::GetLineNumber() const{ return std::to_string(m_line_num);}
    std::string LogAdditionInfo::GetThreadId() const{
        std::stringstream ss;
        ss << m_thread_id;
        return ss.str();
    }
    std::string LogAdditionInfo::GetDate() const{
        char tmp_buf[FOMATE_STR_BUFFER_SIZE];
        sprintf(tmp_buf, "%04d/%02d/%02d", m_tm->tm_year + 1900, m_tm->tm_mon + 1, m_tm->tm_mday);
        return std::string(tmp_buf);
    }
    std::string LogAdditionInfo::GetHours() const{
        return std::to_string(m_tm->tm_hour);
    }
    std::string LogAdditionInfo::GetMinutes() const{
        return std::to_string(m_tm->tm_min);
    }
    std::string LogAdditionInfo::GetSeconds() const{
        return std::to_string(m_tm->tm_sec);
    }
    std::string LogAdditionInfo::GetMilliseconds() const{
        return std::to_string(m_time.tv_nsec / MICROSECONDS_PER_MILLISECONDS);
    }

    std::string LogAdditionInfo::GetAdditionInfoByPlaceholder(char plchld) const{
        return GetAdditionInfoByPlaceholder(placeholder::to_enum(plchld));
    }

    std::string LogAdditionInfo::GetAdditionInfoByPlaceholder(placeholder::PlaceholderType plchld) const{
        using PlcFunc = std::function<std::string(void)>;
        using PairOfHoldAndFunc = std::pair<placeholder::PlaceholderType, PlcFunc>;
        static std::vector<PairOfHoldAndFunc> s_placeholder2func = {
            std::make_pair(placeholder::n_NEW_LINE,       []() -> std::string {return "\n";}),
            std::make_pair(placeholder::t_TAB,            []() -> std::string {return "\t";}),
            std::make_pair(placeholder::T_THREAD_ID,      std::bind(&LogAdditionInfo::GetThreadId, this)),
            std::make_pair(placeholder::F_FILE_NAME,      std::bind(&LogAdditionInfo::GetFileName, this)),
            std::make_pair(placeholder::L_LINE,           std::bind(&LogAdditionInfo::GetLineNumber, this)),
            std::make_pair(placeholder::P_PRIORITY,       []() -> std::string {return "Unsupported";}),
            std::make_pair(placeholder::C_CONTENT,        []() -> std::string {return "Unsupported";}),
            std::make_pair(placeholder::D_DATE,           std::bind(&LogAdditionInfo::GetDate, this)),
            std::make_pair(placeholder::H_HOUR,           std::bind(&LogAdditionInfo::GetHours,this)),
            std::make_pair(placeholder::M_MINUTE,         std::bind(&LogAdditionInfo::GetMinutes,this)),
            std::make_pair(placeholder::S_SECOND,         std::bind(&LogAdditionInfo::GetSeconds,this)),
            std::make_pair(placeholder::s_MILLISECOND,    std::bind(&LogAdditionInfo::GetMilliseconds,this)),
            std::make_pair(placeholder::MAX_PATTERN,      []() -> std::string {return "Unsupported";}),
        };
        auto target = std::lower_bound(s_placeholder2func.begin(), s_placeholder2func.end(), 
                                        plchld, 
                                        [](const PairOfHoldAndFunc& a, placeholder::PlaceholderType b)->bool{
                                            return a.first < b;
                                        });
        if(target == s_placeholder2func.end() || target->first != plchld){
            std::cout << ("Unspported type" + std::string(1, placeholder::to_char(plchld)));
            return "Unsupported";
        }
        return (target->second)();
    }

  /*********************class LoggerPattern**************************************/
    LoggerPattern::LoggerPattern(){
        m_pattern_str = LoggerPattern::GetDefaultPatternStr();
    }

    LoggerPattern::LoggerPattern(const std::string& pt):m_pattern_str(pt){}

    // 格式化+模式化
    std::string LoggerPattern::FmtAndPatternLog(LogAdditionInfo::ptr other_info, 
                                                level::LevelEnum lv, 
                                                const std::string& org_str, 
                                                va_list strArgs){
        return PatternLog(other_info, lv, FormatLog(org_str, strArgs));
    }

    // 模式化
    std::string LoggerPattern::PatternLog(LogAdditionInfo::ptr other_info, 
                                            level::LevelEnum lv, 
                                            const std::string &content){
        std::string ans = "";
        for(int i = 0; i < m_pattern_str.length(); i++){
            if(m_pattern_str[i] == '%' && i < m_pattern_str.length() - 1){
                placeholder::PlaceholderType plType = placeholder::to_enum(m_pattern_str[i+1]);
                if(plType == placeholder::C_CONTENT){
                    ans += content;
                }else if(plType == placeholder::P_PRIORITY){
                    ans += level::to_string(lv);
                }
                else{
                    ans += other_info->GetAdditionInfoByPlaceholder(plType);
                }
                i++;
            }else{
                ans += m_pattern_str[i];
            }
        }
        return ans;
    }

    // 格式化
    std::string LoggerPattern::FormatLog(const std::string &org_str, ...){
        va_list strArgs;
        va_start(strArgs, org_str);
        std::string ans = FormatLog(org_str, strArgs);
        va_end(strArgs);
        return ans;
    }

    std::string LoggerPattern::FormatLog(const std::string &org_str, va_list strArgs){
        char buffer[FOMATE_STR_BUFFER_SIZE];
        int rc = vsprintf(buffer, org_str.c_str(), strArgs);
        if(rc < 0){
            // ERROR. TODO
            return "";
        }
        std::string content(buffer);
        return content;
    }

    // (static) 获取默认模式串
    std::string LoggerPattern::GetDefaultPatternStr() {
        return DEFAULT_PATTERN_STR;
    }

    void LoggerPattern::SetPatternStr(const std::string & str){
        m_pattern_str = str;
    }
    
    std::string LoggerPattern::GetPatternStr(){
        return m_pattern_str;
    }

    void LoggerPattern::Reset2Default(){
        m_pattern_str = LoggerPattern::GetDefaultPatternStr();
    }

    /*********************class LoggerSinkInterface**************************************/
    LoggerSinkInterface::LoggerSinkInterface(const std::string& name) : m_name(name){}

    LoggerSinkInterface::~LoggerSinkInterface(){}

    std::string LoggerSinkInterface::GetName(){
        return m_name;
    }
    
    StdLoggerSink::~StdLoggerSink(){ m_stream = nullptr; }

    StdLoggerSink::StdLoggerSink(const std::string& name, StdLoggerSinkType tp)
                                : LoggerSinkInterface(name), m_type(tp)
    {
        switch(tp){
            case STD_COUT:
                m_stream = &std::cout;
                break;
            case STD_ERROR:
                m_stream = &std::cerr;
                break;
            default:
                m_stream = &std::cout;
        }
    }
    
    void StdLoggerSink::Sink(const std::string& content){
        (*m_stream) << content << std::endl;
    }

    StdLoggerSinkType StdLoggerSink::GetType(){
        return m_type;
    }

    FileLoggerSink::FileLoggerSink(const std::string& name, const std::string& file_name)
                                    : LoggerSinkInterface(name), m_file_name(file_name) 
    {
        Reopen();
    }

    FileLoggerSink::~FileLoggerSink(){
        if(m_stream){
            m_stream.close();
        }
    }

    void FileLoggerSink::Sink(const std::string& content){
        m_stream << content << std::endl;
    }

    bool FileLoggerSink::Reopen(){
        if(m_stream) {
            m_stream.close();
        }
        m_stream.open(m_file_name, std::ios::app);
        return m_stream.is_open();
    }

    /*********************class Logger**************************************/
    /// 构造函数、拷贝构造、赋值构造
    Logger::Logger(const std::string& name){
        m_name = name;
        this->Reset();
    }

    Logger::Logger(const std::string &name, level::LevelEnum lv, const std::string& pt)
                    :m_name(name), m_level(lv)
    {
        m_pattern = std::make_shared<LoggerPattern>(pt);
    }

    /// 落日志
    void Logger::LogImpl(LogAdditionInfo::ptr other_info, 
                    level::LevelEnum lv, 
                    const std::string& org_str){
        if(m_sinks.empty() || lv < m_level){
            // std::cout << "logger named [" << m_name << "] have no sink!" << std::endl;
            return;
        }

        std::string final_str = org_str;
        if(other_info != nullptr){
            final_str = m_pattern->PatternLog(other_info, lv, final_str);
        }

        for(const auto& single_sink : m_sinks){
            (single_sink.second)->Sink(final_str);
        }
    }

    void Logger::LogImplf(LogAdditionInfo::ptr other_info, 
                    level::LevelEnum lv, 
                    const std::string& org_str, 
                    va_list vargs){
        std::string formatted_str = m_pattern->FormatLog(org_str, vargs);
        LogImpl(other_info, lv, formatted_str);
    }

    // no format, no pattern
    void Logger::Log(level::LevelEnum lv, 
                const std::string& str){
        LogImpl(nullptr, lv, str);
    }

    // format, no pattern
    void Logger::Logf(level::LevelEnum lv, 
                const std::string& org_str, ...){
        va_list str_args;
        va_start(str_args, org_str);
        LogImplf(nullptr, lv, org_str, str_args);
        va_end(str_args);
    }

    // no format, pattern
    void Logger::Log(LogAdditionInfo::ptr other_info, 
                    level::LevelEnum lv, 
                    const std::string& str){
        LogImpl(other_info, lv, str);
    }

    // format, pattern
    void Logger::Logf(LogAdditionInfo::ptr other_info, 
                        level::LevelEnum lv, 
                        const std::string& org_str, ...){
        va_list str_args;
        va_start(str_args, org_str);
        LogImplf(other_info, lv, org_str, str_args);
        va_end(str_args);
    }

    /// 辅助函数
    void Logger::Reset(){
        m_level = level::INFO;
        m_sinks.clear();
        m_pattern.reset(new LoggerPattern());
        m_pattern->Reset2Default();
    }

    const std::string Logger::GetName(){
        return m_name;
    }
    
    /// 日志模式相关
    LoggerPattern::ptr Logger::GetPattern() { return m_pattern; }
    void Logger::SetPattern(const std::string& pattern_str){ 
        m_pattern->SetPatternStr(pattern_str);
    }
    void Logger::SetPattern(LoggerPattern::ptr pattern_ptr){ 
        m_pattern = pattern_ptr;
    }
    
    /// 日志级别相关
    void Logger::SetLevel(level::LevelEnum lv){
        m_level = lv;
    }

    void Logger::SetLevel(const std::string &lv){
        m_level = level::to_enum(lv);
    }

    level::LevelEnum Logger::GetLevel(){
        return m_level;
    }

    /// 日志sink相关
    LoggerSinkInterface::ptr Logger::GetLoggerSink(const std::string &name){
        return m_sinks[name];
    }

    void Logger::AddSink(LoggerSinkInterface::ptr sink){
        m_sinks.insert(std::make_pair(sink->GetName(), sink));
    }

    void Logger::DelSink(const std::string &name){
        m_sinks.erase(name);
    }
    
    void Logger::CleanSink(){
        m_sinks.clear();
    }
  
    /*********************class LoggerManger**************************************/
    /**
     * @brief Construct a new Logger Manger:: Logger Manger object. 
     *        1. clear all loggers.
     *        2. create&init a new logger named "root" as default logger.
     *        3. set default logger.
     * 
     */
    LoggerManger::LoggerManger(){
        m_loggers.clear();
        if(m_default_logger != nullptr){
            m_default_logger->Reset();
        }
        m_default_logger = std::make_shared<dysv::Logger>(DEFAULT_LOGGER_NAME);
        
        m_default_logger->AddSink(std::make_shared<StdLoggerSink>(STD_COUT_NAME, STD_COUT));
        m_loggers[m_default_logger->GetName()] = m_default_logger;
    }

    Logger::ptr LoggerManger::GetDefaultLog(){
        return m_default_logger;
    }

    void LoggerManger::SetDefaultLog(Logger::ptr logger){
        m_default_logger = logger;
    }

    bool LoggerManger::AddLogger(Logger::ptr logger){
        m_loggers.insert(std::pair<std::string, Logger::ptr>(logger->GetName(), logger));
    }

    bool LoggerManger::DelLogger(const std::string &name){
        m_loggers.erase(name);
    }

    Logger::ptr LoggerManger::GetLogger(const std::string& name){
        return m_loggers[name];
    }

    // no format, no pattern
    void trace(const std::string &str){
        DEFAULT_LOGGER->Log(dysv::level::TRACE, str);
    }
    void info(const std::string &str){
        DEFAULT_LOGGER->Log(dysv::level::INFO, str);
    }
    void warn(const std::string &str){
        DEFAULT_LOGGER->Log(dysv::level::WARN, str);
    }
    void error(const std::string &str){
        DEFAULT_LOGGER->Log(dysv::level::ERROR, str);
    }
    void fatal(const std::string &str){
        DEFAULT_LOGGER->Log(dysv::level::FATAL, str);
    }

    // format, no pattern
    void fmt_trace(const std::string &str, ...){
        va_list vargs;
        va_start(vargs, str);
        DEFAULT_LOGGER->LogImplf(nullptr, dysv::level::TRACE, str, vargs);
        va_end(vargs);
    }
    void fmt_info(const std::string &str, ...){
        va_list vargs;
        va_start(vargs, str);
        DEFAULT_LOGGER->LogImplf(nullptr, dysv::level::INFO, str, vargs);
        va_end(vargs);
    }
    void fmt_warn(const std::string &str, ...){
        va_list vargs;
        va_start(vargs, str);
        DEFAULT_LOGGER->LogImplf(nullptr, dysv::level::WARN, str, vargs);
        va_end(vargs);
    }
    void fmt_error(const std::string &str, ...){
        va_list vargs;
        va_start(vargs, str);
        DEFAULT_LOGGER->LogImplf(nullptr, dysv::level::ERROR, str, vargs);
        va_end(vargs);
    }
    void fmt_fatal(const std::string &str, ...){
        va_list vargs;
        va_start(vargs, str);
        DEFAULT_LOGGER->LogImplf(nullptr, dysv::level::FATAL, str, vargs);
        va_end(vargs);
    }

    void set_level(level::LevelEnum lv){
        SET_LEVEL(lv);
    }
    void set_default_logger(Logger::ptr lg){
        SET_DEFAULT_LOGGER(lg);
    }
    void set_default_pattern(const std::string &str){
        SET_DEFAULT_PATTERN(str);
    }
    void add_sink(LoggerSinkInterface::ptr sink){
        ADD_SINK(sink);
    }
    void clean_sink(){
        CLEAN_SINK();
    }
}