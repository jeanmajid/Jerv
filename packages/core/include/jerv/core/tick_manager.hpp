#pragma once

#include <jerv/core/job_manager.hpp>
#include <chrono>
#include <atomic>
#include <thread>
#include <functional>

namespace jerv::core {
    class TickManager {
    public:
        JobManager jobManager;
        std::function<void()> pollNetwork;

        uint64_t currentTick() const { return currentTick_; }
        std::chrono::steady_clock::time_point currentTickStartTime() const { return currentTickStartTime_; }
        int ticksPerSecond() const { return ticksPerSecond_; }

        void start();
        void stop();
        void setTicksPerSecond(int tps);
    private:
        void tick();

        uint64_t currentTick_ = 0;
        std::chrono::steady_clock::time_point currentTickStartTime_;
        int ticksPerSecond_ = 20;
        std::chrono::milliseconds millisecondsPerTick_{50};
        std::atomic<bool> running_{false};
    };
}