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

#pragma once
#include <cstdint>
#include <vector>

#include "blockStorage.hpp"

namespace jerv::core::world::generator {
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
