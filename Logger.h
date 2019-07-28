//
// Created by w0wy on 7/28/2019.
//

#ifndef THREADS_LOGGER_H
#define THREADS_LOGGER_H

#include <cstdarg>
#include <string>
#include <fstream>
#include <mutex>

namespace logging {

    inline int _vscprintf (const char * format, va_list pargs) {
        int retval;
        va_list argcopy;
        va_copy(argcopy, pargs);
        retval = vsnprintf(NULL, 0, format, argcopy);
        va_end(argcopy);
        return retval;
    }

    class Logger {
    private:
        Logger() = default;
        Logger &operator=(const Logger &);
        Logger(const Logger &);

        static const std::string logFileName_;
        static Logger *this_;
        static std::ofstream logFileStream_;
        static std::mutex locker_;
    public:
        void log(const std::string&);
        void log(const char *, ...);
        Logger& operator<<(const std::string&);

        static Logger* getLogger();
    };
}

#endif //THREADS_LOGGER_H
