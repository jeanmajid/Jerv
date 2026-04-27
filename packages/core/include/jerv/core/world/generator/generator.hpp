#pragma once
#include <vector>

#include "chunk.hpp"

namespace jerv::core::world::generator {
    class ChunkGenerator {
    public:
        std::pair<std::vector<protocol::ChunkCoords>, std::vector<Chunk *> > generateChunks(
            protocol::Vec3f location, int32_t chunkRadius);

        Chunk *generateChunk(int32_t chunkX, int32_t chunkZ);

        std::string getChunkKey(int32_t chunkX, int32_t chunkY);

    private:
        std::unordered_map<std::string, Chunk> chunks;
    };
}
