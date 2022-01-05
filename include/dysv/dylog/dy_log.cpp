#include "dysv/dy_log.hpp"

namespace dysv{
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
}                               \

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
    }

    void trace(const std::string& str){
        std::cout << str << std::endl;
    }
}