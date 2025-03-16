
#include <spdlog/spdlog.h>
#include "spdlog/async.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include <spdlog/fmt/bundled/printf.h>

#include <thread>
#include <iostream>

class LogUtils
{
private:
    LogUtils(/* args */)
    {
        spdlog::init_thread_pool(8192, 1); // 8K 1-thread
        auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        stdout_sink->set_level(spdlog::level::info); // 只有消息类型大于等于 设置值的才会写入到 sink中
        auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("mylog.txt", 1024 * 1024 * 10, 3);
        rotating_sink->set_level(spdlog::level::trace);
        // TODO logcat
        std::vector<spdlog::sink_ptr> sinks{stdout_sink, rotating_sink};
        auto logger = std::make_shared<spdlog::async_logger>("camsys", sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);

        logger->set_level(spdlog::level::trace); // 日志记录器的全局日志级别为 trace
        spdlog::register_logger(logger);
    }

public:
    static std::shared_ptr<spdlog::logger> GetInstance()
    {
        static LogUtils instance;
        return spdlog::get("camsys");
    }

    ~LogUtils() {}
};

template <class loggerPtr, class... Args>
void loglineprintf(loggerPtr logger,
                   spdlog::level::level_enum level,
                   spdlog::source_loc loc,
                   const char *fmt,
                   const Args &...args) noexcept
{
    if (logger && logger->should_log(level))
    {
        logger->log(loc, level, "{}", fmt::sprintf(fmt, args...));
    }
}

template <class loggerPtr, class... Args>
void logprintf(loggerPtr logger,
                   spdlog::level::level_enum level,
                   const char *fmt,
                   const Args &...args) noexcept
{
    if (logger && logger->should_log(level))
    {
        logger->log(level, "{}", fmt::sprintf(fmt, args...));
    }
}
/*#ifndef SPDLOG_NO_SOURCE_LOC
    #define SPDLOG_LOGGER_CALL(logger, level, ...) \
        (logger)->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, level, __VA_ARGS__)
#else
    #define SPDLOG_LOGGER_CALL(logger, level, ...) \
        (logger)->log(spdlog::source_loc{}, level, __VA_ARGS__)
#endif*/




#define SPDLOG_LINE_LOGGER_PRINTF(logger, level, ...) \
    loglineprintf(logger, level, spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, __VA_ARGS__)

#define SPDLOG_LOGGER_PRINTF(logger, level, ...) \
    logprintf(logger, level,  __VA_ARGS__)

// specific log implementation macros
#define LOG_TRACE(...)      SPDLOG_LOGGER_PRINTF(LogUtils::GetInstance(), spdlog::level::trace, __VA_ARGS__)
#define LOG_DEBUG(...)      SPDLOG_LOGGER_PRINTF(LogUtils::GetInstance(), spdlog::level::debug, __VA_ARGS__)
#define LOG_INFO(...)       SPDLOG_LOGGER_PRINTF(LogUtils::GetInstance(), spdlog::level::info, __VA_ARGS__)
#define LOG_WARN(...)       SPDLOG_LOGGER_PRINTF(LogUtils::GetInstance(), spdlog::level::warn, __VA_ARGS__)
#define LOG_ERROR(...)      SPDLOG_LOGGER_PRINTF(LogUtils::GetInstance(), spdlog::level::err, __VA_ARGS__)
#define LOG_CRITICAL(...)   SPDLOG_LOGGER_PRINTF(LogUtils::GetInstance(), spdlog::level::critical, __VA_ARGS__)

#define LOG_LINE_TRACE(...)      SPDLOG_LINE_LOGGER_PRINTF(LogUtils::GetInstance(), spdlog::level::trace, __VA_ARGS__)
#define LOG_LINE_DEBUG(...)      SPDLOG_LINE_LOGGER_PRINTF(LogUtils::GetInstance(), spdlog::level::debug, __VA_ARGS__)
#define LOG_LINE_INFO(...)       SPDLOG_LINE_LOGGER_PRINTF(LogUtils::GetInstance(), spdlog::level::info, __VA_ARGS__)
#define LOG_LINE_WARN(...)       SPDLOG_LINE_LOGGER_PRINTF(LogUtils::GetInstance(), spdlog::level::warn, __VA_ARGS__)
#define LOG_LINE_ERROR(...)      SPDLOG_LINE_LOGGER_PRINTF(LogUtils::GetInstance(), spdlog::level::err, __VA_ARGS__)
#define LOG_LINE_CRITICAL(...)   SPDLOG_LINE_LOGGER_PRINTF(LogUtils::GetInstance(), spdlog::level::critical, __VA_ARGS__)

 
#define LOG_TRACE_FMT(...)      SPDLOG_LOGGER_TRACE(LogUtils::GetInstance(),  __VA_ARGS__)
#define LOG_DEBUG_FMT(...)      SPDLOG_LOGGER_DEBUG(LogUtils::GetInstance(),  __VA_ARGS__)
#define LOG_INFO_FMT(...)       SPDLOG_LOGGER_INFO(LogUtils::GetInstance(),  __VA_ARGS__)
#define LOG_WARN_FMT(...)       SPDLOG_LOGGER_WARN(LogUtils::GetInstance(),  __VA_ARGS__)
#define LOG_ERROR_FMT(...)      SPDLOG_LOGGER_ERROR(LogUtils::GetInstance(),  __VA_ARGS__)
#define LOG_CRITICAL_FMT(...)   SPDLOG_LOGGER_CRITICAL(LogUtils::GetInstance(),  __VA_ARGS__)

#define LOG_LINE_TRACE_FMT(...)      SPDLOG_LOGGER_CALL(LogUtils::GetInstance(), spdlog::level::trace, __VA_ARGS__)
#define LOG_LINE_DEBUG_FMT(...)      SPDLOG_LOGGER_CALL(LogUtils::GetInstance(), spdlog::level::debug, __VA_ARGS__)
#define LOG_LINE_INFO_FMT(...)       SPDLOG_LOGGER_CALL(LogUtils::GetInstance(), spdlog::level::info, __VA_ARGS__)
#define LOG_LINE_WARN_FMT(...)       SPDLOG_LOGGER_CALL(LogUtils::GetInstance(), spdlog::level::warn, __VA_ARGS__)
#define LOG_LINE_ERROR_FMT(...)      SPDLOG_LOGGER_CALL(LogUtils::GetInstance(), spdlog::level::err, __VA_ARGS__)
#define LOG_LINE_CRITICAL_FMT(...)   SPDLOG_LOGGER_CALL(LogUtils::GetInstance(), spdlog::level::critical, __VA_ARGS__)


// 线程函数，用于记录日志
void logInThread()
{
    try
    {
        auto logger = LogUtils::GetInstance();
        // 记录日志
        for (int i = 0; i < 100; ++i)
        {
            
            logger->info("Logging from thread: Message {}", i);
            logger->trace("This is a trace message->");
            logger->debug("This is a debug message->");
            logger->info("This is an info message->");
            logger->warn("This is a warning message->");
            logger->error("This is an error message->");
            logger->critical("This is a critical message.");
           
            LOG_TRACE("define LOG_TRACE %d",i);
            LOG_DEBUG("define LOG_DEBUG %d",i);
            LOG_INFO("define LOG_INFO %d",i);
            LOG_WARN("define LOG_WARN %d",i);
            LOG_ERROR("define LOG_ERROR %d",i);
            LOG_CRITICAL("define LOG_CRITICAL %d",i);

            LOG_LINE_TRACE("define LOG_TRACE %d",i);
            LOG_LINE_DEBUG("define LOG_DEBUG %d",i);
            LOG_LINE_INFO("define LOG_INFO %d",i);
            LOG_LINE_WARN("define LOG_WARN %d",i);
            LOG_LINE_ERROR("define LOG_ERROR %d",i);
            LOG_LINE_CRITICAL("define LOG_CRITICAL %d",i);


            LOG_TRACE_FMT("define LOG_TRACE_FMT {}",i);
            LOG_DEBUG_FMT("define LOG_DEBUG_FMT {}",i);
            LOG_INFO_FMT("define LOG_INFO_FMT {}",i);
            LOG_WARN_FMT("define LOG_WARN_FMT {}",i);
            LOG_ERROR_FMT("define LOG_ERROR_FMT {}",i);
            LOG_CRITICAL_FMT("define LOG_CRITICAL_FMT {}",i);

            LOG_LINE_TRACE_FMT("define LOG_LINE_TRACE_FMT {}",i);
            LOG_LINE_DEBUG_FMT("define LOG_LINE_DEBUG_FMT {}",i);
            LOG_LINE_INFO_FMT("define LOG_LINE_INFO_FMT {}",i);
            LOG_LINE_WARN_FMT("define LOG_LINE_WARN_FMT {}",i);
            LOG_LINE_ERROR_FMT("define LOG_LINE_ERROR_FMT {}",i);
            LOG_LINE_CRITICAL_FMT("define LOG_LINE_CRITICAL_FMT {}",i);

            std::this_thread::sleep_for(std::chrono::seconds(1)); // 线程休眠 1 秒
        }
    }
    catch (const spdlog::spdlog_ex &ex)
    {
        std::cout << "Log initialization failed: " << ex.what() << std::endl;
    }
}
int main()
{

    spdlog::info("Welcome to spdlog!");
    SPDLOG_DEBUG("Some debug message");
    std::thread loggingThread(logInThread);
    std::thread loggingThread2(logInThread);
    std::thread loggingThread3(logInThread);
    loggingThread.join();
    loggingThread2.join();
    loggingThread3.join();

    return 0;
}