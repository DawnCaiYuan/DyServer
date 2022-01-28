#include "dysv/dy_log.hpp"

namespace dysv{
    #define FOMATE_STR_BUFFER_SIZE  4096

    /*******************Interface Method******************************************/
    void trace(const std::string& str){
        DY_LOG_TRACE(str);
    }

    void warn(const std::string& str){
        DY_LOG_WARN(str);
    }

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

    /*******************class LogAddtionInfo***********************************/
    LogAddtionInfo::LogAddtionInfo(const std::string& file, uint64_t line)
                                    : m_file_name(file), m_line(line){
        m_thread_id = std::this_thread::get_id();
        m_time = time(0);
        m_elapse = 0;
    }
    std::string LogAddtionInfo::GetFileName() const {return m_file_name;}
    std::string LogAddtionInfo::GetLineNumber() const{ return m_line;}
    std::string LogAddtionInfo::GetThreadId() const{return m_thread_id;}
    std::string LogAddtionInfo::GetDate() const{return "YYYY/MM/DD";}
    std::string LogAddtionInfo::GetHours() const{return "HH";}
    std::string LogAddtionInfo::GetMinutes() const{return "MM"};
    std::string LogAddtionInfo::GetSeconds() const{return "SS"};
    std::string LogAddtionInfo::GetMilliseconds() const{return "sss"};
    std::string LogAddtionInfo::GetAdditionInfoByPlaceholder(char plchld) const{
        return GetAdditionInfoByPlaceholder(placeholder::to_enum(plchld));
    }
    std::string LogAddtionInfo::GetAdditionInfoByPlaceholder(PlaceholderType plchld) const{
        using PlcFunc = std::string (*)();
        using PairOfHoldAndFunc = std::pair<PlaceholderType, PlcFunc>;
        static std::vector<pairOfHoldAndFunc> s_placeholder2func = {
            {placeholder::n_NEW_LINE,       []() -> std::string {return "\n";}},
            {placeholder::t_TAB,            []() -> std::string {return "\t";}},
            {placeholder::T_THREAD_ID,      GetThreadId},
            {placeholder::F_FILE_NAME,      GetFileName},
            {placeholder::L_LINE,           GetLineNumber},
            {placeholder::P_PRIORITY,       []() -> std::string {return "Unsupported"},
            {placeholder::C_CONTENT,        []() -> std::string {return "Unsupported"},
            {placeholder::D_DATE,           GetDate},
            {placeholder::H_HOUR,           GetHours},
            {placeholder::M_MINUTE,         GetMinutes},
            {placeholder::S_SECOND,         GetSeconds},
            {placeholder::s_MILLISECOND,    GetMilliseconds},
            {placeholder::MAX_PATTERN,      []() -> std::string {return "Unsupported"},
        };
        auto target = std::lower_bound(s_placeholder2func.begin(), s_placeholder2func.end(), 
                                        plchld, 
                                        [](const PairOfHoldAndFunc& a, placeholder::PlaceholderType b)->bool{
                                            return a.first < b;
                                        });
        if(target == s_pl_handles.end() || target.first != plchld){
            std::cout << ("Unspported type" + std::string(1, placeholder::to_char(pl)));
            return "Unsupported";
        }
        return (target->second)();
    }

  /*********************class LoggerPattern**************************************/
    LoggerPattern::LoggerPattern(){
        m_pattern_str = LoggerPattern::GetDefaultPatternStr();
    }

    std::string LoggerPattern::FmtAndPatternLog(LogAdditionInfo::ptr other_info, 
                                                level::LevelEnum lv, 
                                                const std::string& org_str, 
                                                va_list strArgs){
        return PatternLog(other_info, lv, FormatLog(org_str, strArgs));
    }

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
                    ans += other_info.GetAdditionInfoByPlaceholder(plType);
                }
                i++;
            }else{
                ans += m_pattern_str[i];
            }
        }
        return ans;
    }

    std::string LoggerPattern::FormatLog(const std::string &org_str, ...){
        va_list strArgs;
        va_start(strArgs, org_str);
        string ans = FormatLog(org_str, strArgs);
        va_end(strArgs);
        return ans;
    }

    std::string LoggerPattern::FormatLog(const std::string &org_str, va_list strArgs){
        char buffer[FOMATE_STR_BUFFER_SIZE];
        int rc = vsprintf(buffer, org_str, strArgs);
        if(rc < 0){
            // ERROR. TODO
            return;
        }
        string content(buffer);
        return content;
    }

    std::string LoggerPattern::GetDefaultPatternStr() {
        return DEFAULT_PATTERN_STR;
    }

    void LoggerPattern::SetPatternStr(const std::string & str)){
        m_pattern_str = str;
    }
    
    std::string LoggerPattern::GetPatternStr(){
        return m_pattern_str;
    }

    void LoggerPattern::Reset2Default(){
        m_pattern_str = LoggerPattern::GetDefaultPatternStr();
    }

    /*********************class Logger**************************************/
    Logger::Logger(const std::string& name){
        m_name = name;
        Logger::Reset();
    }

    LoggerPattern::ptr Logger::GetPattern() const { return m_pattern; }
    void Logger::SetPattern(const std::string& placeholder){ 
        // TODO
    }
    void Logger::SetPattern(LoggerPattern::ptr placeholder){ 
        m_pattern = placeholder;
    }
    

    void Logger::SetLevel(level::LevelEnum lv){
        m_level = lv;
    }

    void Logger::SetLevel(const std::string &lv){
        m_level = level::to_enum(lv);
    }

    level::LevelEnum Logger::GetLevel(){
        return m_level;
    }


    void Logger::Reset(){
        m_level = level::INFO;
        m_sinks.clear();
        m_pattern = std::make_shared<LoggerPattern>();
        m_pattern->Reset2Default();
    }

    const std::string Logger::GetName(){
        return m_name;
    }

    void Logger::AddSink(LoggerSink::ptr sink){
        m_sinks.push_back(sink);
    }

    /**
     * @brief 通过传入的信息，将日志落入各sink中。
     *          1. 判断若无sink或者日志级别不够则直接返回。
     *          2. 将日志格式化。("hello %s", "dawn") -> "hello dawn".
     *          3. 将日志模式化。[2022/01/27][INFO][hello dawn]
     *          4. 将日志交由sink落地。
     * 
     * @param other_info 日志模式化时所需额外信息。通常包含文件名、行号等。
     * @param lv 日志级别
     * @param org_str 日志内容。例如,"Year %d, hello %s".
     * @param ... 日志内容所需信息。例如, "2022", "dawn cy".
     */
    void Logger::Log(LogAddtionInfo::ptr other_info, 
                        level::LevelEnum lv, 
                        const std::string& org_str, ...){
        if(m_sinks.empty() || lv < m_level){
            // std::cout << "logger named [" << m_name << "] have no sink!" << std::endl;
            return;
        }

        va_list strArgs;
        va_start(strArgs, org_str);
        std::string formatted_str = m_pattern->FmtAndPatternLog(other_info, lv, org_str, strArgs);
        va_end(strArgs);

        for(const auto& single_sink : m_sinks){
            (single_sink.second)->Sink(formatted_str);
        }
    }

    /*********************class LoggerSinkInterface**************************************/
    LoggerSinkInterface::GetName(){
        m_stream = &std::cout;
    }

    void LoggerSink::Sink(const std::string& str){
        (*m_stream) << str << std::endl;
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
        
        m_default_logger->AddSink(std::make_shared<LoggerSink>());
        m_loggers[m_default_logger->GetName()] = m_default_logger;
    }

    Logger::ptr LoggerManger::GetDefaultLog(){
        return m_default_logger;
    }

}