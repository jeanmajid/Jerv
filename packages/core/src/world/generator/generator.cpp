#include "jerv/core/world/generator/generator.hpp"

#include "jerv/raknet/serverConnection.hpp"

namespace jerv::core::world::generator {
    struct ChunkOffset {
        int32_t dx, dz;
        int32_t distSq;

        bool operator<(const ChunkOffset &other) const {
            return distSq < other.distSq;
        }
    };

    std::pair<std::vector<protocol::ChunkCoords>, std::vector<Chunk *> > ChunkGenerator::generateChunks(
        raknet::ServerConnection &connection) {
        // TODO: All of this definetly still has lots of optimisation potential
        // TODO: Unload all the chunks for players who disconnect
        const int32_t centerChunkX = static_cast<int32_t>(std::floor(connection.playerLocationX / 16.0f));
        const int32_t centerChunkZ = static_cast<int32_t>(std::floor(connection.playerLocationZ / 16.0f));

        const int32_t radiusSq = connection.playerViewDistance * connection.playerViewDistance;
        std::vector<ChunkOffset> offsets;
        offsets.reserve(3.14159f * radiusSq);

        for (int32_t dx = -connection.playerViewDistance; dx <= connection.playerViewDistance; ++dx) {
            for (int32_t dz = -connection.playerViewDistance; dz <= connection.playerViewDistance; ++dz) {
                int32_t distSq = dx * dx + dz * dz;
                if (distSq <= radiusSq) {
                    offsets.push_back({dx, dz, distSq});
                }
            }
        }

        std::sort(offsets.begin(), offsets.end());

        constexpr uint32_t maxChunksToSend = 8;
        std::vector<Chunk *> generatedChunks;
        std::vector<protocol::ChunkCoords> coords;

        generatedChunks.reserve(maxChunksToSend);
        coords.reserve(maxChunksToSend);

        uint32_t chunksSend = 0;

        std::unordered_set<uint64_t> desiredChunks;
        desiredChunks.reserve(offsets.size());

        for (const auto &offset: offsets) {
            int32_t chunkX = centerChunkX + offset.dx;
            int32_t chunkZ = centerChunkZ + offset.dz;

            desiredChunks.insert(getChunkKey(chunkX, chunkZ));
        }

        for (auto it = connection.playerLoadedChunks.begin();
             it != connection.playerLoadedChunks.end();) {
            if (!desiredChunks.contains(*it)) {
                auto chunkIt = chunks.find(*it);
                if (chunkIt != chunks.end()) {
                    Chunk &chunk = chunkIt->second;

                    if (--chunk.viewers == 0) {
                        chunks.erase(getChunkKey(chunk.chunkX, chunk.chunkZ));
                    }
                }

                it = connection.playerLoadedChunks.erase(it);
            } else {
                ++it;
            }
        }

        for (const auto &offset: offsets) {
            if (chunksSend >= maxChunksToSend) {
                break;
            }

            int32_t chunkX = centerChunkX + offset.dx;
            int32_t chunkZ = centerChunkZ + offset.dz;

            uint64_t chunkKey = getChunkKey(chunkX, chunkZ);

            auto [it, inserted] = connection.playerLoadedChunks.insert(chunkKey);
            if (!inserted) {
                continue;
            }

            Chunk *chunk = generateChunk(chunkX, chunkZ, chunkKey);
            ++chunk->viewers;
            ++chunksSend;

            coords.emplace_back(chunkX, chunkZ);
            generatedChunks.emplace_back(chunk);
        }

        return {std::move(coords), std::move(generatedChunks)};
    }

    Chunk *ChunkGenerator::generateChunk(int32_t chunkX, int32_t chunkZ, const uint64_t chunkKey) {
        const auto [it, inserted] = chunks.try_emplace(chunkKey, chunkX, chunkZ);
        if (inserted) {
            for (int32_t x = 0; x < 16; ++x) {
                for (int32_t z = 0; z < 16; ++z) {
                    for (int32_t y = 0; y < 100; ++y) {
                        it->second.setBlock(x, y, z, -567203660);
                    }
                }
            }
        }

        return &it->second;
    }

    uint64_t ChunkGenerator::getChunkKey(const int32_t chunkX, const int32_t chunkZ) {
        return static_cast<uint64_t>(chunkX) << 32 | static_cast<uint32_t>(chunkZ);
    }
}
