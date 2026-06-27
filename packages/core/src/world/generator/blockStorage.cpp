/* SPDX-License-Identifier: LGPL-3.0-or-later
 * ============================================================================
 *  Jerv - Minecraft Bedrock Server Software
 *  Copyright (C) 2025-2026 jeanmajid
 *  https://github.com/jeanmajid/Jerv
 * ============================================================================
 *
 * This file is part of Jerv.
 *
 * Jerv is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Jerv is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Jerv. If not, see <https://www.gnu.org/licenses/>.
 */

#include "jerv/core/world/generator/blockStorage.hpp"

namespace jerv::core::world::generator {
    BlockStorage::BlockStorage() {
        palette.push_back(0);
        blocks.resize(MAX_SIZE, 0);
    }

    bool BlockStorage::isEmpty() {
        return palette.size() == 1 && palette[0] == 0;
    }

    int32_t BlockStorage::getState(const int32_t x, const int32_t y, const int32_t z) {
        return palette[blocks[getIndex(x, y, z)]];
    }

    void BlockStorage::setState(const int32_t x, const int32_t y, const int32_t z, const int32_t state) {
        const auto it = std::ranges::find(palette, state);
        size_t paletteIndex;
        if (it == palette.end()) {
            paletteIndex = palette.size();
            palette.push_back(state);
        } else {
            paletteIndex = it - palette.begin();
        }

        blocks[getIndex(x, y, z)] = paletteIndex;
    }

    void BlockStorage::serialize(jerv::binary::ResizableCursor &cursor) {
        int32_t bitsPerBlock = std::ceil(std::log2(std::max(static_cast<size_t>(2), palette.size())));

        if (bitsPerBlock <= 0) bitsPerBlock = 1;
        else if (bitsPerBlock > 6 && bitsPerBlock <= 8) bitsPerBlock = 8;
        else if (bitsPerBlock > 8) bitsPerBlock = 16;

        const int32_t blocksPerWord = 32 / bitsPerBlock;
        const int32_t wordCount = (4096 + blocksPerWord - 1) / blocksPerWord;

        const size_t neededSize = 1 + (wordCount * 4) + 5 + (palette.size() * 5);
        cursor.growToFit(neededSize);

        cursor.writeUint8(static_cast<uint8_t>(bitsPerBlock << 1 | 1));

        static constexpr int32_t MAX_BLOCKS = 4096;

        for (int32_t w = 0; w < wordCount; w++) {
            uint32_t word = 0;
            for (int32_t block = 0; block < blocksPerWord; block++) {
                const int32_t index = w * blocksPerWord + block;
                if (index >= MAX_BLOCKS) break;
                const uint32_t state = blocks[index];
                word |= state << (block * bitsPerBlock);
            }
            cursor.writeUint32<true>(word);
        }

        cursor.writeZigZag32(static_cast<int32_t>(palette.size()));

        for (const int32_t state: palette) {
            cursor.writeZigZag32(state);
        }
    }

    size_t BlockStorage::getIndex(const int32_t x, const int32_t y, const int32_t z) {
        return ((x & 0xF) << 8) | ((z & 0xF) << 4) | (y & 0xF);
    }
}
