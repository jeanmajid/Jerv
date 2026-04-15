#pragma once
#include <cstdint>
#include <vector>

#include "jerv/binary/cursor.hpp"

namespace jerv::core::world {
    class BlockStorage {
    public:
        static constexpr int32_t MAX_X = 16;
        static constexpr int32_t MAX_Y = 16;
        static constexpr int32_t MAX_Z = 16;
        static constexpr int32_t MAX_SIZE = MAX_X * MAX_Y * MAX_Z;

        BlockStorage();

        bool isEmpty();

        int32_t getState(int32_t x, int32_t y, int32_t z);

        void setState(int32_t x, int32_t y, int32_t z, int32_t state);

        void serialize(jerv::binary::ResizableCursor &cursor);

    private:
        size_t getIndex(int32_t x, int32_t y, int32_t z);

        std::vector<int32_t> palette;
        std::vector<uint16_t> blocks;
    };

}
