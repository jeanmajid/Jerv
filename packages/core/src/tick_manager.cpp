#include <jerv/core/tick_manager.hpp>
#include <iostream>

namespace jerv::core {
    void TickManager::start() {
        running_ = true;

        while (running_) {
            tick();
            currentTick_++;
        }
    }

    void TickManager::stop() {
        running_ = false;
    }

    void TickManager::tick() {
        currentTickStartTime_ = std::chrono::steady_clock::now();

        if (pollNetwork) {
            pollNetwork();
        }

        jobManager.tick(currentTick_);
        jobManager.processJobs();

        const auto elapsed = std::chrono::steady_clock::now() - currentTickStartTime_;
        auto remaining = millisecondsPerTick_ - std::chrono::duration_cast<std::chrono::milliseconds>(elapsed);

        if (remaining.count() > 0) {
            std::this_thread::sleep_for(remaining);
        }
    }

    void TickManager::setTicksPerSecond(int tps) {
        ticksPerSecond_ = tps;
        millisecondsPerTick_ = std::chrono::milliseconds(1000 / tps);
    }
}