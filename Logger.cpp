//
// Created by w0wy on 7/28/2019.
//

#include <stdio.h>

#include "Logger.h"

// THIS SHIT NOT WORKING
const std::string logging::Logger::logFileName_ = "syslog.txt";
logging::Logger* logging::Logger::this_ = nullptr;
std::ofstream logging::Logger::logFileStream_;
std::mutex logging::Logger::locker_;

logging::Logger* logging::Logger::getLogger() {
    if (this_ == nullptr)
    {
        this_ = new Logger();
        logFileStream_.open(logFileName_.c_str(), std::ofstream::out | std::ofstream::app);
    }

    return this_;
}

void logging::Logger::log(const char * format, ...)
{
    locker_.lock();
    char * message = nullptr;
    int length = 0;
    va_list args;
    va_start(args, format);
    //  Return the number of characters in the string referenced the list of arguments.
    // _vscprintf doesn't count terminating '\0' (that's why +1)
    length = _vscprintf(format, args) + 1;
    message = new char[length];
    vsprintf(message, format, args);
    logFileStream_ << "\n" << message << "\n";
    va_end(args);

    delete [] message;
    locker_.unlock();
}

void logging::Logger::log(const std::string & message)
{
    locker_.lock();
    logFileStream_ <<"\n" << message << "\n";
    locker_.unlock();
}

logging::Logger& logging::Logger::operator<<(const std::string& message)
{
    locker_.lock();
    logFileStream_ << "\n" << message << "\n";
    locker_.unlock();
    return *this;
}