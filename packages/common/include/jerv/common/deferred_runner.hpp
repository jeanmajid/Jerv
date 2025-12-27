#pragma once

#include <functional>
#include <atomic>

namespace jerv::common {
    class deferred_runner {
    public:
        using ScheduleFunc = std::function<void(std::function < void() >)>;
        using TaskFunc = std::function<void()>;

        deferred_runner(ScheduleFunc schedule, TaskFunc task)
            : schedule_(std::move(schedule))
              , task_(std::move(task))
              , isQueued_(false)
              , wasQueued_(false) {
        }

        void defer() {
            isQueued_.store(true, std::memory_order_release);

            bool expected = false;
            if (wasQueued_.compare_exchange_strong(expected, true, std::memory_order_relaxed)) {
                schedule_([this]() {
                    bool shouldRun = isQueued_.exchange(false, std::memory_order_relaxed);
                    wasQueued_.store(false, std::memory_order_release);

                    if (shouldRun) {
                        task_();
                    }
                });
            }
        }

        void run() {
            isQueued_.store(false, std::memory_order_release);
            task_();
        }

    private:
        ScheduleFunc schedule_;
        TaskFunc task_;
        std::atomic<bool> isQueued_;
        std::atomic<bool> wasQueued_;
    };
}