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
