#pragma once

#include <jerv/binary/cursor.hpp>
#include <vector>
#include <cstdint>
#include <cmath>
#include <algorithm>

namespace jerv::core {
    class BlockStorage {
    public:
        static constexpr int32_t MAX_X = 16;
        static constexpr int32_t MAX_Y = 16;
        static constexpr int32_t MAX_Z = 16;
        static constexpr int32_t MAX_SIZE = 16 * 16 * 16;

        BlockStorage() {
            palette_.push_back(0);
            blocks_.resize(MAX_SIZE, 0);
        }

        bool isEmpty() const {
            return palette_.size() == 1 && palette_[0] == 0;
        }

        int32_t getState(const int32_t bx, const int32_t by, const int32_t bz) const {
            const size_t index = getIndex(bx, by, bz);
            const size_t paletteIndex = blocks_[index];
            return palette_[paletteIndex];
        }

        void setState(const int32_t bx, const int32_t by, const int32_t bz, const int32_t state) {
            const auto it = std::ranges::find(palette_, state);
            size_t paletteIndex;
            if (it == palette_.end()) {
                paletteIndex = palette_.size();
                palette_.push_back(state);
            } else {
                paletteIndex = static_cast<size_t>(it - palette_.begin());
            }
            blocks_[getIndex(bx, by, bz)] = static_cast<uint16_t>(paletteIndex);
        }

        static size_t getIndex(const int32_t bx, const int32_t by, const int32_t bz) {
            return static_cast<size_t>(((bx & 0xF) << 8) | ((bz & 0xF) << 4) | (by & 0xF));
        }

        void serialize(binary::ResizableCursor &cursor) const {
            int32_t bitsPerBlock = static_cast<int32_t>(std::ceil(std::log2(std::max(static_cast<size_t>(2), palette_.size()))));

            if (bitsPerBlock <= 0) bitsPerBlock = 1;
            else if (bitsPerBlock > 6 && bitsPerBlock <= 8) bitsPerBlock = 8;
            else if (bitsPerBlock > 8) bitsPerBlock = 16;

            const int32_t blocksPerWord = 32 / bitsPerBlock;
            const int32_t wordCount = (4096 + blocksPerWord - 1) / blocksPerWord;

            const size_t neededSize = 1 + (wordCount * 4) + 5 + (palette_.size() * 5);
            cursor.growToFit(neededSize);

            cursor.writeUint8(static_cast<uint8_t>((bitsPerBlock << 1) | 1));

            constexpr int32_t MAX_BLOCKS = 4096;

            for (int32_t w = 0; w < wordCount; w++) {
                uint32_t word = 0;
                for (int32_t block = 0; block < blocksPerWord; block++) {
                    const int32_t index = w * blocksPerWord + block;
                    if (index >= MAX_BLOCKS) break;
                    const uint32_t state = blocks_[index];
                    word |= (state << (block * bitsPerBlock));
                }
                cursor.writeUint32(word, true);
            }

            cursor.writeZigZag32(static_cast<int32_t>(palette_.size()));

            for (const int32_t state: palette_) {
                cursor.writeZigZag32(state);
            }
        }

        const std::vector<int32_t> &palette() const { return palette_; }
        const std::vector<uint16_t> &blocks() const { return blocks_; }
    private:
        std::vector<int32_t> palette_;
        std::vector<uint16_t> blocks_;
    };
}