//
// Created by w0wy on 7/28/2019.
//

#include <cstring>
#include <iostream>
#include <unistd.h>
#include <stdio.h>

#include "Logger.h"

namespace smartlog
{

std::string Logger::logFileName_;
std::ofstream Logger::logFileStream_;
std::mutex Logger::locker_;

Logger::Logger(Level l):
    level_(l)
{
    if (logFileName_.empty())
    {
        char currentPath[FILENAME_MAX];
        getcwd(currentPath, FILENAME_MAX);
        logFileName_ = currentPath;
        logFileName_ += "/syslog.log";
    }

    if (!logFileStream_.is_open())
        logFileStream_.open(logFileName_.c_str(), std::ofstream::out | std::ofstream::trunc); // | std::ofstream::app);
}

// TODO implement other severities
// TODO try to show stack trace for FATAL severity
void Logger::operator()(std::string const& message,
    char const* date,
    char const* time,
    char const* file,
    char const* function,
    int line)
{
    std::lock_guard<std::mutex> lock(locker_);
    switch(level_)
    {
        case Level::Debug:
        {
            std::string tempFile(file);
            std::string withoutSyslog = logFileName_.substr(0, logFileName_.find_last_of("/"));
            std::string actualFilePath = tempFile.substr(withoutSyslog.find_last_of("/") + 1);

            logFileStream_ 
                << "DBG - [" << date 
                << "][" << time 
                << "] [" << actualFilePath 
                << "] " << function 
                << ":" << line 
                << " # " << message
                << std::endl;
        }
        break;
        case Level::Info:
        default:
        {
            logFileStream_ 
                << "INFO - [" << date 
                << "][" << time 
                << "] --- " << message
                << std::endl;
        }
    }
}

}  // namespace smartlog