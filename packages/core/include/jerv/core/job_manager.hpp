#pragma once

#include <functional>
#include <vector>
#include <map>
#include <set>
#include <memory>

namespace jerv::core {
    using TaskFunc = std::function<void()>;

    class JobManager {
    public:
        std::function<void(const std::exception &)> onError;

        void runTimeout(TaskFunc func, int delay = 1);
        void run(TaskFunc func);
        void tick(uint64_t tick);
        bool processJobs();
    private:
        std::vector<TaskFunc> runs_;
        std::map<uint64_t, std::vector<TaskFunc>> records_;
        std::set<std::shared_ptr<std::function<bool()>>> jobs_;
        uint64_t currentTick_ = 0;
    };
}