/* SPDX-License-Identifier: LGPL-3.0-or-later
 * ============================================================================
 *  Jerv - Minecraft Bedrock Server Software
 *  Copyright (C) 2025-2026 jeanmajid
 *  https://github.com/jeanmajid/Jerv
 * ============================================================================
 *
 * This file is part of Jerv.
 *
 * Jerv is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Jerv is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Jerv. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <memory>

namespace jerv::common {
    class Logger {
    public:
        static void setLevel(const spdlog::level::level_enum level) {
            logger->set_level(level);
        }

        template<typename... Args>
        static void info(spdlog::format_string_t<Args...> fmt, Args &&... args) {
            logger->info(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static void debug(spdlog::format_string_t<Args...> fmt, Args &&... args) {
            logger->debug(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static void warn(spdlog::format_string_t<Args...> fmt, Args &&... args) {
            logger->warn(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static void trace(spdlog::format_string_t<Args...> fmt, Args &&... args) {
            logger->trace(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static void error(spdlog::format_string_t<Args...> fmt, Args &&... args) {
            logger->error(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static void critical(spdlog::format_string_t<Args...> fmt, Args &&... args) {
            logger->critical(fmt, std::forward<Args>(args)...);
        }

    private:
        inline static auto logger = [] {
            auto log = spdlog::stdout_color_mt("Jerver");
            log->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%^%l%$] %v");
            log->set_level(spdlog::level::debug);
            return log;
        }();
    };
}
