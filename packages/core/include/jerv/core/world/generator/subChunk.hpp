#pragma once
#include <cstdint>
#include <vector>

#include "blockStorage.hpp"

namespace jerv::core::world {
    class SubChunk {
    public:
        SubChunk() = default;

        BlockStorage& getLayer(size_t index = 0);

        int32_t getState(int32_t x, int32_t y, int32_t z, size_t layer = 0);

        void setState(int32_t x, int32_t y, int32_t z, int32_t state, size_t layer = 0);

        bool isEmpty();

        void serialize(jerv::binary::ResizableCursor &cursor);

    private:
        static constexpr uint8_t VERSION = 8;
        std::vector<BlockStorage> layers;
    };
}
