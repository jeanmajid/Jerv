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
#include <vector>

#include "chunk.hpp"
#include "jerv/raknet/serverConnection.hpp"
#include "jerv/core/world/generator/levelDB.hpp"

namespace jerv::core::world::generator {
    class ChunkGenerator {
    public:
        std::pair<std::vector<protocol::ChunkCoords>, std::vector<Chunk *> > generateChunks(raknet::ServerConnection &connection
        );

        Chunk *generateChunk(int32_t chunkX, int32_t chunkZ, uint64_t chunkKey);

        uint64_t getChunkKey(int32_t chunkX, int32_t chunkZ);

    private:
        std::unordered_map<uint64_t, Chunk> chunks;

        LevelDB levelDB;
    };
}
