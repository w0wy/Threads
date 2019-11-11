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

class Logger {
public:
    Logger(Level l);

    void operator()(std::string const& message,
        char const* date,
        char const* time,
        char const* file,
        char const* function,
        int line);

    void operator()(std::string const& tag,
        std::string const& message,
        char const* date,
        char const* time,
        char const* file,
        char const* function,
        int line);

private:
    void dumpOutput(
        std::string const& tag,
        std::string const& message,
        char const* date,
        char const* time,
        char const* file,
        char const* function,
        int line);
    std::string getLevel();

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

#define LOG_TAG(Logger_, Tag_, Message_)             \
    Logger_(                                         \
        static_cast<std::ostringstream&>(            \
            std::ostringstream().flush() << Tag_     \
        ).str(),                                     \
        static_cast<std::ostringstream&>(            \
            std::ostringstream().flush() << Message_ \
        ).str(),                                     \
        __DATE__,                                    \
        __TIME__,                                    \
        __FILE__,                                    \
        __PRETTY_FUNCTION__,                         \
        __LINE__                                     \
    );    

inline Logger& Debug() {
    static Logger logger_dbg(Level::Debug);
    return logger_dbg;
}

inline Logger& Info() {
    static Logger logger_info(Level::Info);
    return logger_info;
}

inline Logger& Warning() {
    static Logger logger_wrn(Level::Warning);
    return logger_wrn;
}

inline Logger& Error() {
    static Logger logger_err(Level::Error);
    return logger_err;
}

inline Logger& Fatal() {
    static Logger logger_fatal(Level::Fatal);
    return logger_fatal;
}

#ifdef __DEBUG__ // use this when compiling with param "-DDEBUG=ON"
#    define LOG_DEBUG(Message_) LOG(smartlog::Debug(), Message_)
#    define LOG_DEBUG_T(Tag_, Message_) LOG_TAG(smartlog::Debug(), Tag_, Message_)
#else
#    define LOG_DEBUG(Message_) do {} while(0)
#    define LOG_DEBUG_T(Tag_, Message_) do {} while(0)
#endif

#define LOG_INFO(Message_) LOG(smartlog::Info(), Message_)
#define LOG_INFO_T(Tag_, Message_) LOG_TAG(smartlog::Info(), Tag_, Message_)

#define LOG_WRN(Message_) LOG(smartlog::Warning(), Message_)
#define LOG_WRN_T(Tag_, Message_) LOG_TAG(smartlog::Warning(), Tag_, Message_)

#define LOG_ERR(Message_) LOG(smartlog::Error(), Message_)
#define LOG_ERR_T(Tag_, Message_) LOG_TAG(smartlog::Error(), Tag_, Message_)

#define LOG_FATAL(Message_) LOG(smartlog::Fatal(), Message_)
#define LOG_FATAL_T(Tag_, Message_) LOG_TAG(smartlog::Fatal(), Tag_, Message_)

}  // namespace smartlog

#endif //THREADS_LOGGER_H
