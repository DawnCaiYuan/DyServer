#include <cstdio>
#include "dysv/dy_log.hpp"

int main(){
    /*just cout what you give.*/
    // dysv::warn("Hello! Canary.");
    //console: Hello! Canary.


    /*format the log*/
    //dysv::warn("Hello! %s.", Canary);
    //console: Hello! Canary.
    
    /*using default output pattern*/
    // dysv::DY_LOG_WARN("this is warning message!");  
    //console: [2022/01/12 18:30:50:888][154][/home/dysv/example/example.cpp][14][WARN][this is a warning message!]
    
    /*format the log*/
    dysv::DY_LOG_WARN("%s answer is %d!", "Ultimate", 42);
    //console: [2022/01/12 18:30:50:888][154][/home/dysv/example/example.cpp][8][WARN][Ultimate answer is 42!]
    
//   /*change default logger's pattern*/
//   dysv::SET_DEFAULT_PATTERN("[%D %H:%M:%S:%s][%P][%C]");
//   //no output
//
//   /*log again*/
//   dysv::DY_LOG_WARN("i have changed the output patter!");
//   //console: [2022/01/12 18:30:50:888][WARN][i have changed the output patter!]
//
//   /*change default logger's sink. let's log flow into "file.txt" */
//   dysv::SET_DEFAULT_SINK("./file.txt", 'a');
//   //no output
//
//   /*log in file*/
//   dysv::DY_LOG_WARN("file log write success!");
//   //file.txt: [2022/01/12 18:30:50:888][WARN][file log write success!]
//
//   /*log both in file and console*/
//   dysv::DEFAULT_LOGGER_ADD_SINK(STD_COUT, 'a');
//   // no output
//
//   /*log in file and console*/
//   dysv::DY_LOG_WARN("Shadow Clone!");
//   //console: [2022/01/12 18:30:50:888][WARN][Shadow Clone!]
//   //file.txt:[2022/01/12 18:30:50:888][WARN][Shadow Clone!]
//
//   /*customize logger && log level*/
//   auto file_logger = dysv::Logger("mySpLogger", level::WARN, "[%P][%C]");
//   file_logger->AddSink(std::make_shared<LoggerSink>()); // add default sink, log will flow into console.
//   file_logger->Trace("hello %s", "dy");   // not written
//   file_logger->Warn("sad emerging...");   // written.
//   file_logger->SetLevel(level::TRACE);
//   file_logger->Trace("hello %s", "dy");   // written.

    return 0;
}

