#pragma once
#include <cstdint>
#include <map>
#include <ranges>
#include <span>
#include <vector>

namespace jerv::raknet {
    class FragmentMeta {
    public:
        void set(uint32_t index, std::span<uint8_t> data) {
            if (fragments.contains(index)) {
                return;
            }
            fragments[index] = std::vector(data.begin(), data.end());
            totalSize += data.size();
        }

        size_t length() const {
            return fragments.size();
        }

        std::vector<uint8_t> build() const {
            std::vector<uint8_t> result;
            result.reserve(totalSize);

            for (const auto &fragment: fragments | std::views::values) {
                result.insert(result.end(), fragment.begin(), fragment.end());
            }

            return result;
        }

    private:
        std::map<uint32_t, std::vector<uint8_t> > fragments;
        size_t totalSize = 0;
    };
}