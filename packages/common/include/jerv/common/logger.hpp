#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <memory>

namespace jerv::common {
    class Logger {
    public:
        static spdlog::logger& getInstance() {
            static auto logger = [] {
                auto log = spdlog::stdout_color_mt("Jerver");
                log->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%^%l%$] %v");
                return log;
            }();
            return *logger;
        }
    };

#define JERV_LOG_TRACE(...) jerv::common::Logger::getInstance().trace(__VA_ARGS__)
#define JERV_LOG_DEBUG(...) jerv::common::Logger::getInstance().debug(__VA_ARGS__)
#define JERV_LOG_INFO(...) jerv::common::Logger::getInstance().info(__VA_ARGS__)
#define JERV_LOG_WARN(...) jerv::common::Logger::getInstance().warn(__VA_ARGS__)
#define JERV_LOG_ERROR(...) jerv::common::Logger::getInstance().error(__VA_ARGS__)
#define JERV_LOG_CRITICAL(...) jerv::common::Logger::getInstance().critical(__VA_ARGS__)
}
