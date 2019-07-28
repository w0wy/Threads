//
// Created by w0wy on 7/28/2019.
//

#include "Logger.h"

const std::string logging::Logger::logFileName_ = "syslog.log";
logging::Logger* logging::Logger::this_ = nullptr;
std::ofstream logging::Logger::logFileStream_;
std::mutex logging::Logger::locker_;

logging::Logger* logging::Logger::getLogger() {
    std::lock_guard<std::mutex> guard(locker_);
    if (this_ == nullptr)
    {
        this_ = new Logger();
        logFileStream_.open(logFileName_.c_str(), std::ofstream::out | std::ofstream::app);
    }

    return this_;
}

void logging::Logger::log(const std::string & message)
{
    logFileStream_ << message << std::endl;
}

logging::Logger& logging::Logger::operator<<(const std::string& message)
{
    logFileStream_ << message << std::endl;
    return *this;
}