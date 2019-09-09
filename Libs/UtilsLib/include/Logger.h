//
// Created by w0wy on 7/28/2019.
//

#ifndef THREADS_LOGGER_H
#define THREADS_LOGGER_H

#include <cstdarg>
#include <string>
#include <fstream>
#include <mutex>
#include <sstream>

namespace smartlog
{

enum class Level
{
    Fatal,
    Error,
    Warning,
    Info,
    Debug
};

// TODO make logger not use pointers => no leaks
class Logger {
public:
    Logger(Level l);

    void operator()(std::string const& message,
        char const* date,
        char const* time,
        char const* file,
        char const* function,
        int line);

private:
    Level level_;
    static std::ofstream logFileStream_;
    static std::mutex locker_;
    static std::string logFileName_;
};

#define LOG(Logger_, Message_)                       \
    Logger_(                                         \
        static_cast<std::ostringstream&>(            \
            std::ostringstream().flush() << Message_ \
        ).str(),                                     \
        __DATE__,                                    \
        __TIME__,                                    \
        __FILE__,                                    \
        __PRETTY_FUNCTION__,                         \
        __LINE__                                     \
    );                                               \

Logger& Debug() {
    static Logger logger_dbg(Level::Debug);
    return logger_dbg;
};

Logger& Info() {
    static Logger logger_info(Level::Info);
    return logger_info;
};

Logger& Warning() {
    static Logger logger_wrn(Level::Warning);
    return logger_wrn;
};

Logger& Error() {
    static Logger logger_err(Level::Error);
    return logger_err;
};

Logger& Fatal() {
    static Logger logger_fatal(Level::Fatal);
    return logger_fatal;
};

#ifdef __DEBUG__ // use this when compiling with param "-DDEBUG=ON"
#    define LOG_DEBUG(Message_) LOG(smartlog::Debug(), Message_)
#else
#    define LOG_DEBUG(_) do {} while(0)
#endif

#define LOG_INFO(Message_) LOG(smartlog::Info(), Message_)
#define LOG_WRN(Message_) LOG(smartlog::Warning(), Message_)
#define LOG_ERR(Message_) LOG(smartlog::Error(), Message_)
#define LOG_FATAL(Message_) LOG(smartlog::Fatal(), Message_)

}  // namespace smartlog

#endif //THREADS_LOGGER_H
