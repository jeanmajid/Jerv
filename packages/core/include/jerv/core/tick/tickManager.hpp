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
#include <cstdint>
#include <chrono>
#include <atomic>
#include <thread>

namespace jerv::core::tick {
    class TickManager {
    public:
        TickManager();

        void start();

        void stop();

        void setTicksPerSecond(int tps);

        uint64_t currentTick = 0;

        using Callback = void(*)(void*, uint64_t);
        void setCallback(void* ctx, const Callback cb) {
            context = ctx;
            callback = cb;
        }
    private:
        void run();

        std::chrono::steady_clock::time_point currentTickStartTime;
        int ticksPerSecond = 20;
        std::chrono::duration<double> interval{1.0 / ticksPerSecond};

        std::atomic<bool> running{false};
        std::thread thread;

        void* context = nullptr;
        Callback callback = nullptr;
    };
}
