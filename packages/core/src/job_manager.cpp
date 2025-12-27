#include <jerv/core/job_manager.hpp>

namespace jerv::core {
    void JobManager::runTimeout(TaskFunc func, int delay) {
        delay = std::max(0, delay);
        records_[currentTick_ + delay].push_back(std::move(func));
    }

    void JobManager::run(TaskFunc func) {
        runs_.push_back(std::move(func));
    }

    void JobManager::tick(const uint64_t tick) {
        currentTick_ = tick;

        const auto runs = std::move(runs_);
        runs_.clear();

        for (auto &func: runs) {
            try {
                func();
            } catch (const std::exception &e) {
                if (onError) {
                    onError(e);
                }
            }
        }

        auto it = records_.find(tick);
        if (it != records_.end()) {
            for (auto &func: it->second) {
                try {
                    func();
                } catch (const std::exception &e) {
                    if (onError) {
                        onError(e);
                    }
                }
            }
            records_.erase(it);
        }
    }

    bool JobManager::processJobs() {
        std::vector<std::shared_ptr<std::function<bool()> > > toRemove;

        for (auto &job: jobs_) {
            try {
                if ((*job)()) {
                    toRemove.push_back(job);
                }
            } catch (const std::exception &e) {
                if (onError) {
                    onError(e);
                }
                toRemove.push_back(job);
            }
        }

        for (auto &job: toRemove) {
            jobs_.erase(job);
        }

        return !jobs_.empty();
    }
}