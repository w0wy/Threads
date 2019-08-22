//
// Created by w0wy on 7/28/2019.
//

#include "Logger.h"

const std::string Logger::logFileName_ = "syslog.log";
Logger* Logger::this_ = nullptr;
std::ofstream Logger::logFileStream_;
std::mutex Logger::locker_;
std::string Logger::tag_;

Logger* Logger::getLogger() {
    std::lock_guard<std::mutex> guard(locker_);
    if (this_ == nullptr)
    {
        this_ = new Logger();
        logFileStream_.open(logFileName_.c_str(), std::ofstream::out); // | std::ofstream::app);
    }

    return this_;
}

void Logger::print(const std::string & message)
{
    logFileStream_ << "[" << tag_  << "] " << message << std::endl;
}

Logger& Logger::operator<<(const std::string& message)
{
    logFileStream_ << "[" << tag_  << "] " << message << std::endl;
    return *this;
}

void Logger::setTag(const std::string & tag)
{
    tag_ = tag;
}