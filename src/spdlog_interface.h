
#include <spdlog/spdlog.h>
#include "spdlog/async.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include <spdlog/fmt/bundled/printf.h>

class SpdLogInterFace
{
private:
    SpdLogInterFace(/* args */)
    {
        spdlog::init_thread_pool(8192, 1); // 8K 1-thread
        auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        stdout_sink->set_level(spdlog::level::info); // 只有消息类型大于等于 设置值的才会写入到 sink中
        auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("mylog.txt", 1024 * 1024 * 10, 30);
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
        static SpdLogInterFace instance;
        return spdlog::get("camsys");
    }

    ~SpdLogInterFace() {}
};

template <class loggerPtr, class... Args>
void log_line_printf(loggerPtr logger,
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
void log_printf(loggerPtr logger,
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

#ifndef SPDLOG_NO_SOURCE_LOC

#define SPDLOG_LOGGER_PRINTF(logger, level, ...) \
    log_line_printf(logger, level, spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, __VA_ARGS__)
#else
#define SPDLOG_LOGGER_PRINTF(logger, level, ...) \
    log_printf(logger, level, __VA_ARGS__)
#endif

// specific log implementation macros
#define LOG_TRACE(...) SPDLOG_LOGGER_PRINTF(SpdLogInterFace::GetInstance(), spdlog::level::trace, __VA_ARGS__)
#define LOG_DEBUG(...) SPDLOG_LOGGER_PRINTF(SpdLogInterFace::GetInstance(), spdlog::level::debug, __VA_ARGS__)
#define LOG_INFO(...) SPDLOG_LOGGER_PRINTF(SpdLogInterFace::GetInstance(), spdlog::level::info, __VA_ARGS__)
#define LOG_WARN(...) SPDLOG_LOGGER_PRINTF(SpdLogInterFace::GetInstance(), spdlog::level::warn, __VA_ARGS__)
#define LOG_ERROR(...) SPDLOG_LOGGER_PRINTF(SpdLogInterFace::GetInstance(), spdlog::level::err, __VA_ARGS__)
#define LOG_CRITICAL(...) SPDLOG_LOGGER_PRINTF(SpdLogInterFace::GetInstance(), spdlog::level::critical, __VA_ARGS__)

#define LOG_TRACE_FMT(...) SPDLOG_LOGGER_TRACE(SpdLogInterFace::GetInstance(), __VA_ARGS__)
#define LOG_DEBUG_FMT(...) SPDLOG_LOGGER_DEBUG(SpdLogInterFace::GetInstance(), __VA_ARGS__)
#define LOG_INFO_FMT(...) SPDLOG_LOGGER_INFO(SpdLogInterFace::GetInstance(), __VA_ARGS__)
#define LOG_WARN_FMT(...) SPDLOG_LOGGER_WARN(SpdLogInterFace::GetInstance(), __VA_ARGS__)
#define LOG_ERROR_FMT(...) SPDLOG_LOGGER_ERROR(SpdLogInterFace::GetInstance(), __VA_ARGS__)
#define LOG_CRITICAL_FMT(...) SPDLOG_LOGGER_CRITICAL(SpdLogInterFace::GetInstance(), __VA_ARGS__)
