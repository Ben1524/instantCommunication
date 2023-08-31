#ifndef LOG_H
#define LOG_H
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include <memory>
#include <sstream>

#define filename_windows(x) (strrchr(x,'\\')?strrchr(x,'\\')+1:x)

#define LOG(A,B,...) \
Log::getInstance()->oss<<filename_windows(__FILE__ )<<","<<__func__<<","<<__LINE__; \
Log::getInstance()->logPrint(A,B,Log::getInstance()->oss.str(),##__VA_ARGS__); \

#define LOG_DEBUG_CONSLE(...) LOG("consle",spdlog::level::debug,##__VA_ARGS__)
#define LOG_DEBUG_FILE(...) LOG("file",spdlog::level::debug,##__VA_ARGS__)

#define LOG_INFO_CONSLE(...) LOG("consle",spdlog::level::info,##__VA_ARGS__)
#define LOG_INFO_FILE(...) LOG("file",spdlog::level::info,##__VA_ARGS__)

#define LOG_WARN_CONSLE(...) LOG("consle",spdlog::level::warn,##__VA_ARGS__)
#define LOG_WARN_FILE(...) LOG("file",spdlog::level::warn,##__VA_ARGS__)

#define LOG_ERROR_CONSLE(...) LOG("consle",spdlog::level::err,##__VA_ARGS__)
#define LOG_ERROR_FILE(...) LOG("file",spdlog::level::err,##__VA_ARGS__)



class Log
{
public:
    Log();
    using Logptr=std::shared_ptr<Log>;
    static std::shared_ptr<Log>& getInstance();

    std::ostringstream oss;
    template<typename... Args>
    inline void logPrint(const std::string& loggerName="consle",spdlog::level::level_enum l=spdlog::level::debug,const std::string&fl="",const char* format="", Args&&... args)
    {
        oss.str("");
        oss.clear();
        auto s = fmt::format("[{:s}] ", fl);
        auto logger = spdlog::get(loggerName);
        s+=format;
        logger->log(l,s,std::forward<Args>(args)...);
    }

private:
    static std::shared_ptr<Log> log;
    std::shared_ptr<spdlog::logger> consle=spdlog::stdout_color_mt("consle");
    std::shared_ptr<spdlog::logger> file=spdlog::rotating_logger_mt("file","logs/file.log",256,10);
};

#endif // LOG_H
