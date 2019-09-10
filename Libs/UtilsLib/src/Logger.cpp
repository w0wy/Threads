//
// Created by w0wy on 7/28/2019.
//

#include <cstring>
#include <iostream>
#include <unistd.h>
#include <stdio.h>

#include "Utilities.h"

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

void Logger::operator()(std::string const& message,
    char const* date,
    char const* time,
    char const* file,
    char const* function,
    int line)
{
    std::lock_guard<std::mutex> lock(locker_);

    std::string tempFile(file);
    std::string functionSig(function);

    std::string withoutSyslog = logFileName_.substr(0, logFileName_.find_last_of("/"));
    std::string actualFilePath = tempFile.substr(withoutSyslog.find_last_of("/") + 1);
    
    std::string lineStr(std::to_string(line));

    convertField(actualFilePath, 40, ']');
    convertField(functionSig, 52, ' ');
    convertField(lineStr, 4, ' ');

    switch(level_)
    {
        case Level::Debug:
        {
            logFileStream_
                << "[DBG]\t";
        }
        break;
        case Level::Info:
        {
            logFileStream_ 
                << "[INF]\t";
        }
        break;
        case Level::Warning:
        {
            logFileStream_ 
                << "[WRN]\t";
        }
        break;
        case Level::Error:
        {
            logFileStream_ 
                << "[ERR]\t";
        }
        break;
        case Level::Fatal:
        {
            logFileStream_ 
                << "[FAT]\t";
        }
        break;
    }

    logFileStream_
        << "[" << date 
        << "][" << time
        << "] [" << actualFilePath 
        << "\t " << functionSig 
        << ":" << lineStr 
        << " # " << message
        << std::endl;

    if (level_ == Level::Fatal)
        logFileStream_
            << "\n[callstack]:\n"
            << tracing::get_call_trace()
            << std::endl;
}

void Logger::convertField(std::string& convertibleString, int max, char ch)
{
    static const std::string spaces(100, ' ');

    if (convertibleString.size() < max)
    {
        convertibleString += ch;
        convertibleString += spaces.substr(0, max - convertibleString.size() + 1);
    }

    if (convertibleString.size() > max + 1)
        convertibleString = convertibleString.substr(convertibleString.size() - max);
}

}  // namespace smartlog