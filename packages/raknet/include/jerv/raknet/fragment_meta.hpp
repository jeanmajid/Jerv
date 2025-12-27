#pragma once

#include <vector>
#include <map>
#include <span>
#include <cstdint>
#include <ranges>

namespace jerv::raknet {
    class FragmentMeta {
    public:
        void set(uint32_t index, std::span<uint8_t> data) {
            fragments_[index] = std::vector<uint8_t>(data.begin(), data.end());
            totalSize_ += data.size();
        }

        size_t length() const {
            return fragments_.size();
        }

        std::vector<uint8_t> build() const {
            std::vector<uint8_t> result;
            result.reserve(totalSize_);

            for (const auto &fragment: fragments_ | std::views::values) {
                result.insert(result.end(), fragment.begin(), fragment.end());
            }

            return result;
        }

    private:
        std::map<uint32_t, std::vector<uint8_t> > fragments_;
        size_t totalSize_ = 0;
    };
}
