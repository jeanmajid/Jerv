#include "jerv/core/tick/tickManager.hpp"

#include "jerv/common/logger.hpp"

namespace jerv::core::tick {
    TickManager::TickManager() {
    }

    void TickManager::start() {
        running = true;
        thread = std::thread([this]() { run(); });
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
