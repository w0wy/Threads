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
    {
        logFileStream_.open(logFileName_.c_str(), std::ofstream::out | std::ofstream::trunc); // | std::ofstream::app);
    }
}

void Logger::operator()(std::string const& message,
    char const* date,
    char const* time,
    char const* file,
    char const* function,
    int line)
{
    std::lock_guard<std::mutex> lock(locker_);
    dumpOutput("", message, date, time, file, function, line);
}

void Logger::operator()(std::string const& tag,
    std::string const& message,
    char const* date,
    char const* time,
    char const* file,
    char const* function,
    int line)
{
    std::lock_guard<std::mutex> lock(locker_);
    dumpOutput(tag, message, date, time, file, function, line);
}

void Logger::dumpOutput(
    std::string const& tag,
    std::string const& message,
    char const* date,
    char const* time,
    char const* file,
    char const* function,
    int line)
{
    std::string tempFile(file);
    std::string functionSig(function);

    std::string withoutSyslog = logFileName_.substr(0, logFileName_.find_last_of("/"));
    std::string actualFilePath = tempFile.substr(withoutSyslog.find_last_of("/") + 1);

    logFileStream_
        << getLevel()
        << "[" << date 
        << "][" << time;

    if (level_ == Level::Debug)
        logFileStream_
            << "][" << actualFilePath
            << "][" << functionSig
            << ":" << std::to_string(line);

    if (tag != "")
        logFileStream_
            << "] # " << tag
            << " $ ";

    logFileStream_
        << message
        << std::endl;

    if (level_ == Level::Fatal)
        logFileStream_
            << "\n[callstack]:\n"
            << tracing::get_call_trace()
            << std::endl;
}

std::string Logger::getLevel()
{
    switch(level_)
    {
        case Level::Debug:
        {
            return "[DBG]\t";
        }
        break;
        case Level::Info:
        {
            return "[INF]\t";
        }
        break;
        case Level::Warning:
        {
            return "[WRN]\t";
        }
        break;
        case Level::Error:
        {
            return "[ERR]\t";
        }
        break;
        case Level::Fatal:
        {
            return "[FAT]\t";
        }
        break;
    }

    return "";
}

}  // namespace smartlog