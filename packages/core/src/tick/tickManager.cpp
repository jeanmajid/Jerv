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

#include "jerv/core/tick/tickManager.hpp"

#include "jerv/common/logger.hpp"

namespace jerv::core::tick {
    TickManager::TickManager() {
    }

    void TickManager::start() {
        running = true;
        thread = std::thread([this] { run(); });
    }

    void TickManager::stop() {
        running = false;
        if (thread.joinable()) thread.join();
    }

    void TickManager::run() {
        auto next = std::chrono::steady_clock::now();

        while (running) {
            next += std::chrono::duration_cast<std::chrono::steady_clock::duration>(interval);

            currentTickStartTime = std::chrono::steady_clock::now();

            callback(context, currentTick);
            currentTick++;

            auto now = std::chrono::steady_clock::now();

            if (now < next) {
                std::this_thread::sleep_until(next);
            } else {
                next = now;
            }
        }
    }

    void TickManager::setTicksPerSecond(const int tps) {
        ticksPerSecond = tps;
        interval = std::chrono::duration<double>(1.0 / tps);
    }
}
