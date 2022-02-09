#include <cstdio>
#include "dysv/dy_log.hpp"

#define SINK_NAME_TMP_FILE      "name_tmp_file"
#define TMP_FILE_PATH           "./tmp_file.txt"
#define LOGGER_NAME_STD_ERROR   "log2stderr"
#define SINK_NAME_STD_ERROR     "sink2stderr"

int main(){
    /*just cout what you give.*/
    dysv::warn("Hello! Canary.");
    //console: Hello! Canary.

    /*format the log*/
    dysv::fmt_warn("Hello! formated %s.", "Canary");
    //console: Hello! formated Canary. 
    
    /*using default output pattern*/
    DY_LOG_WARN("this is a warning message!");  
    //console:[2022/02/08 12:49:31:394116][8065][/home/dysv/example/example.cpp][17][WARN][this is a warning message!]
    
    /*format the log*/
    DY_LOG_FMT_WARN("%s answer is %d!", "Ultimate", 42);
    //console: [2022/02/08 12:49:31:394228][8065][/home/dysv/example/example.cpp][21][WARN][Ultimate answer is 42!]
    
    /*change default logger's pattern*/
    SET_DEFAULT_PATTERN("[%T][%P][%C][%F][%L][%D %H:%M:%S:%s]");
    //no output

    /*log again*/
    DY_LOG_WARN("i have changed the output patter!");
    //console: [8065][WARN][i have changed the output patter!][/home/dysv/example/example.cpp][29][2022/02/08 12:49:31:394245]

    // /*add default logger's sink. let's log flow into "file.txt" */
    ADD_SINK(std::make_shared<dysv::FileLoggerSink>(SINK_NAME_TMP_FILE, TMP_FILE_PATH));
    // //no output

    // /*log in file and stdout*/
    DY_LOG_WARN("Shadow Clone!");
    // //file:      [8065][WARN][Shadow Clone!][/home/dysv/example/example.cpp][37][2022/02/08 12:49:31:394284]
    // //console:   [8065][WARN][Shadow Clone!][/home/dysv/example/example.cpp][37][2022/02/08 12:49:31:394284]

    // /*del sink of stdout*/
    DEL_SINK(STD_COUT_NAME);

    // /*log in file*/
    DY_LOG_WARN("file log write success!");
    // //file: [8065][WARN][file log write success!][/home/dysv/example/example.cpp][44][2022/02/08 12:49:31:394326]

    // /*customize logger && log level*/
    auto my_logger = std::make_shared<dysv::Logger>(LOGGER_NAME_STD_ERROR, dysv::level::WARN, "[%D %H:%M:%S:%s][%P][%C]");
    my_logger->AddSink(std::make_shared<dysv::StdLoggerSink>(SINK_NAME_STD_ERROR, dysv::STD_ERROR)); // add sink
    my_logger->Logf(dysv::level::TRACE, "hello %s", "dy");   // not written
    my_logger->Log(dysv::level::WARN, "sad emerging...");    // written.
    // // console: sad emerging...
    my_logger->SetLevel(dysv::level::TRACE);
    my_logger->Logf(dysv::level::TRACE, "hello %s", "dy");   // written. 
    // // console: hello dy
    my_logger->Logf(ADD_ADDITION_INFO, dysv::level::TRACE, "just have %s", "fun"); // written. 
    // // console: [2022/02/08 12:49:31:394356][TRACE][just have fun]

    return 0;
}

