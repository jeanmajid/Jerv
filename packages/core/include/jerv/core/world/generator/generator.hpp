#pragma once
#include <vector>

#include "chunk.hpp"
#include "jerv/raknet/serverConnection.hpp"

namespace jerv::core::world::generator {
    class ChunkGenerator {
    public:
        std::pair<std::vector<protocol::ChunkCoords>, std::vector<Chunk *> > generateChunks(raknet::ServerConnection &connection
        );

        Chunk *generateChunk(int32_t chunkX, int32_t chunkZ, uint64_t chunkKey);

        uint64_t getChunkKey(int32_t chunkX, int32_t chunkZ);

    private:
        std::unordered_map<uint64_t, Chunk> chunks;
    };
}
