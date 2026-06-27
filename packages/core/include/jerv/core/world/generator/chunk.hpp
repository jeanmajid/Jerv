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

#include "subChunk.hpp"
#include "jerv/protocol/enums.hpp"
#include "jerv/protocol/packets/levelChunk.hpp"

namespace jerv::protocol {
    enum class DimensionId;
}

namespace jerv::core::world::generator {
    class Chunk {
    public:
        static constexpr int32_t MAX_SUB_CHUNKS = 24;
        static constexpr int32_t OVERWORLD_MIN_Y = -64;
        static constexpr int32_t OVERWORLD_MAX_Y = 319;
        static constexpr int32_t NETHER_MIN_Y = 0;
        static constexpr int32_t NETHER_MAX_Y = 127;
        static constexpr int32_t END_MIN_Y = 0;
        static constexpr int32_t END_MAX_Y = 255;

        Chunk(int32_t x, int32_t z,
              protocol::DimensionId dimension = protocol::DimensionId::Overworld) : subchunks(MAX_SUB_CHUNKS), chunkX(x),
            chunkZ(z), dimension(dimension) {
            switch (dimension) {
                case protocol::DimensionId::Overworld: {
                    minY = OVERWORLD_MIN_Y;
                    maxY = OVERWORLD_MAX_Y;
                    break;
                }
                case protocol::DimensionId::Nether: {
                    minY = NETHER_MIN_Y;
                    maxY = NETHER_MAX_Y;
                    break;
                };
                case protocol::DimensionId::End: {
                    minY = END_MIN_Y;
                    maxY = END_MAX_Y;
                    break;
                };
                default: {
                    minY = OVERWORLD_MIN_Y;
                    maxY = OVERWORLD_MAX_Y;
                    break;
                }
            }
        }

        int32_t getBlock(int32_t x, int32_t y, int32_t z, size_t layer = 0);

        void setBlock(int32_t x, int32_t y, int32_t z, int32_t state, size_t layer = 0);

        protocol::LevelChunkPacket serialize();

        int32_t chunkX;
        int32_t chunkZ;

        uint16_t viewers = 0;
    private:
        int32_t getSubChunkSendCount();

        int32_t yToSubChunkIndex(int32_t y);

        SubChunk *getSubChunkOptional(int32_t index);

        SubChunk &getSubChunk(int32_t index);

        std::vector<std::unique_ptr<SubChunk> > subchunks;

        protocol::DimensionId dimension;

        int32_t minY;
        int32_t maxY;

        std::optional<protocol::LevelChunkPacket> cache;
    };
}
