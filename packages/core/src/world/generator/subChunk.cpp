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

#include "jerv/core/world/generator/subChunk.hpp"

namespace jerv::core::world::generator {
    BlockStorage& SubChunk::getLayer(const size_t index) {
        if (index >= layers.size()) {
            layers.resize(index + 1);
        }
        return layers[index];
    }

    int32_t SubChunk::getState(const int32_t x, const int32_t y, const int32_t z, const size_t layer) {
        if (layer >= layers.size()) return 0;
        return layers[layer].getState(x, y, z);
    }

    void SubChunk::setState(const int32_t x, const int32_t y, const int32_t z, const int32_t state,
                            const size_t layer) {
        getLayer(layer).setState(x, y, z, state);
    }

    bool SubChunk::isEmpty() {
        for (auto &layer: layers) {
            if (!layer.isEmpty()) return false;
        }
        return true;
    }

    void SubChunk::serialize(jerv::binary::ResizableCursor &cursor) {
        cursor.growToFit(2);
        cursor.writeUint8(VERSION);
        cursor.writeUint8(static_cast<uint8_t>(layers.size()));

        for (auto &layer: layers) {
            layer.serialize(cursor);
        }
    }
}
