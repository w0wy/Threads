//
// Created by w0wy on 7/28/2019.
//

#ifndef THREADS_LOGGER_H
#define THREADS_LOGGER_H

#include <cstdarg>
#include <string>
#include <fstream>
#include <mutex>

namespace smartlog
{

class Logger {
    private:
        Logger() = default;
        Logger &operator=(const Logger &);
        Logger(const Logger &);

        static const std::string logFileName_;
        static Logger *this_;
        static std::ofstream logFileStream_;
        static std::mutex locker_;
        static std::string tag_;
    public:
        void print(const std::string&);
        Logger& operator<<(const std::string&);

        void setTag(const std::string&);
        void setFullTag(const std::string&, const int);

        static Logger* getLogger();
};

}  // namespace smartlog

#endif //THREADS_LOGGER_H
