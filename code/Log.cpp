#include "Log.h"
std::shared_ptr<Log> Log::log=nullptr;
Log::Log()
{
    consle->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [thread %t] %v");
    file->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [thread %t] %v");
    spdlog::set_level(spdlog::level::debug);
}

std::shared_ptr<Log>& Log::getInstance()
{
    if(log==nullptr)
    {
        log=std::make_shared<Log>();
    }
    return log;
}

