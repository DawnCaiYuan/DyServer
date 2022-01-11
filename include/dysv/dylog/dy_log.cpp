#include "dysv/dy_log.hpp"

namespace dysv{
    /*********************namespace level**************************************/
    namespace level{
        const std::string to_string(level::LevelEnum lv){
            switch(lv){
#define XX(name)            \
case level::name:           \
    return (#name);

                XX(TRACE);
                XX(INFO);
                XX(WARN);
                XX(ERROR);
                XX(FATAL);
#undef XX
                default:
                    return "UNKNOW";
            }
            return "UNKNOW";
        }

        level::LevelEnum to_enum(const std::string &lv){
#define XX(enumLv, strLv)       \
if(lv == (#strLv)) {              \
    return level::enumLv;       \
}

            XX(TRACE, trace);
            XX(TRACE, TRACE);

            XX(INFO, info);
            XX(INFO, INFO);

            XX(WARN, warn);
            XX(WARN, WARN);

            XX(ERROR, error);
            XX(ERROR, ERROR);
            
            XX(FATAL, fatal);
            XX(FATAL, FATAL);
#undef XX
            return level::UNKNOW;
        }
    } // end of namespace level

    /*********************namespace placeholder**************************************/
    namespace placeholder{
        char to_char(PlaceholderType pt){
            switch(pt){
#define XX(name)            \
case placeholder::name:           \
    return ((#name)[0]);

            XX(n_NEW_LINE);
            XX(t_TAB);
            XX(T_THREAD_ID);
            XX(F_FILE_NAME);
            XX(L_LINE);
            XX(P_PRIORITY);
            XX(C_CONTENT);
            XX(D_DATE);
            XX(H_HOUR);
            XX(M_MINUTE);
            XX(S_SECOND);
            XX(s_MILLISECOND);
#undef XX
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

    /*********************class Logger**************************************/
    Logger::Logger(const std::string& name){
        m_name = name;
        Logger::Reset();
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

    void Logger::Log(level::LevelEnum lv, const std::string& str){
        std::string formatted_str = m_pattern->FormatStr(lv, str);
        for(auto single_sink : m_sinks){
            (single_sink)->Sink(formatted_str);
            
        }
    }

    /*********************class LoggerSink**************************************/
    LoggerSink::LoggerSink(){
        m_stream = &std::cout;
    }

    void LoggerSink::Sink(const std::string& str){
        (*m_stream) << str << std::endl;
    }

    /*********************class LoggerPattern**************************************/
    std::string NewLineHandle(){
        return "\n";
    }

    std::string TabHandle(){
        return "\t";
    }

    std::string ThreadIdHandle(){
        return "ThreadId"; // TODO
    }

    std::string FileNameHandle(){
        return "FileName";
    }

    std::string LineHandle(){
        return "Line";
    }

    std::string DateHandle(){
        return "Date";
    }

    std::string HourHandle(){
        return "HH";
    }
    
    std::string MinuteHandle(){
        return "MM";
    }

    std::string SecondHandle(){
        return "SS";
    }

    std::string MillisecondHandle(){
        return "sss";
    }


    LoggerPattern::LoggerPattern(){
        m_pattern_str = "";
    }

    std::string ReplacePlaceholder(placeholder::PlaceholderType pl){
        static std::vector<PlaceHldHandle> s_pl_handles = {
            {placeholder::n_NEW_LINE, NewLineHandle},
            {placeholder::t_TAB, TabHandle},
            {placeholder::T_THREAD_ID, ThreadIdHandle},
            {placeholder::F_FILE_NAME, FileNameHandle},
            {placeholder::L_LINE, LineHandle},
            {placeholder::D_DATE, DateHandle},
            {placeholder::H_HOUR, HourHandle},
            {placeholder::M_MINUTE, MinuteHandle},
            {placeholder::S_SECOND, SecondHandle},
            {placeholder::s_MILLISECOND, MillisecondHandle},
        };

        auto target = std::lower_bound(s_pl_handles.begin(), s_pl_handles.end(), pl, 
                                        [](const PlaceHldHandle& a, placeholder::PlaceholderType b)->bool{
                                            return a.type < b;
                                        });
        if(target == s_pl_handles.end() || target->type != pl){
            std::cout << ("Unspported type" + std::string(1, placeholder::to_char(pl)));
            return "";
        }
        return target->fGenMsg();
    }
    
    std::string LoggerPattern::FormatStr(level::LevelEnum lv, const std::string &content){
        std::string ans = "";
        for(int i = 0; i < m_pattern_str.length(); i++){
            if(m_pattern_str[i] == '%' && i < m_pattern_str.length() - 1){
                placeholder::PlaceholderType plType = placeholder::to_enum(m_pattern_str[i+1]);
                if(plType == placeholder::C_CONTENT){
                    ans += content;
                }else if(plType == placeholder::P_PRIORITY){
                    ans += level::to_string(lv);
                }else if(plType == placeholder::MAX_PATTERN){
                    ans += ' ';
                }else{
                    ans += ReplacePlaceholder(plType);
                }
                i++;
            }else{
                ans += m_pattern_str[i];
            }
        }
        return ans;
    }


    void LoggerPattern::Reset2Default(){
        m_pattern_str = LoggerPattern::GetDefaultPatternStr();
    }

    std::string LoggerPattern::GetDefaultPatternStr() {
        return "[%D %H:%M:%S:%s][%T][%F][%L][%P][%C]";
    }

    /*********************class LoggerManger**************************************/
    void a(){}
    
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
        m_default_logger = std::make_shared<dysv::Logger>("root");
        
        m_default_logger->AddSink(std::make_shared<LoggerSink>());
        m_loggers[m_default_logger->GetName()] = m_default_logger;
    }

    Logger::ptr LoggerManger::GetDefaultLog(){
        return m_default_logger;
    }


    /*******************Interface Method******************************************/
    void trace(const std::string& str){
        std::cout << str << std::endl;
    }

    void warn(const std::string& str){
        DEFAULT_LOGGER->Log(level::WARN, str);
    }
}