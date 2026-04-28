#include "jerv/core/world/generator/generator.hpp"

namespace jerv::core::world::generator {
    std::pair<std::vector<protocol::ChunkCoords>, std::vector<Chunk *> > ChunkGenerator::generateChunks(
        const protocol::Vec3f location, const int32_t chunkRadius) {
        const int32_t centerChunkX = static_cast<int32_t>(std::floor(location.x / 16.0f));
        const int32_t centerChunkZ = static_cast<int32_t>(std::floor(location.z / 16.0f));

        const int32_t startX = centerChunkX - chunkRadius;
        const int32_t endX = centerChunkX + chunkRadius;
        const int32_t startZ = centerChunkZ - chunkRadius;
        const int32_t endZ = centerChunkZ + chunkRadius;

        const size_t total = static_cast<size_t>(endX - startX + 1) * static_cast<size_t>(endZ - startZ + 1);

        std::vector<Chunk *> generatedChunks;
        std::vector<protocol::ChunkCoords> coords;
        generatedChunks.reserve(total);
        coords.reserve(total);

        uint32_t chunksSend = 0;
        constexpr uint32_t maxChunksToSend = 120;

        for (int32_t r = 0; r <= chunkRadius; ++r) {
            for (int32_t dx = -r; dx <= r; ++dx) {
                for (int32_t dz = -r; dz <= r; ++dz) {
                    if (chunksSend >= maxChunksToSend) {
                        break;
                    }
                    if (std::abs(dx) != r && std::abs(dz) != r) continue;

                    int32_t chunkX = centerChunkX + dx;
                    int32_t chunkZ = centerChunkZ + dz;

                    if (chunkX < startX || chunkX > endX || chunkZ < startZ || chunkZ > endZ)
                        continue;

                    Chunk *chunk = generateChunk(chunkX, chunkZ);
                    if (chunk->hasBeenSent) {
                        continue;
                    }
                    chunk->hasBeenSent = true;
                    generatedChunks.emplace_back(chunk);
                    coords.emplace_back(chunkX, chunkZ);
                    ++chunksSend;
                }
            }
        }

        return {std::move(coords), std::move(generatedChunks)};
    }

    Chunk *ChunkGenerator::generateChunk(const int32_t chunkX, const int32_t chunkZ) {
        const auto [it, inserted] = chunks.try_emplace(getChunkKey(chunkX, chunkZ), chunkX, chunkZ);
        if (inserted) {
            for (int32_t x = 0; x <= 16; ++x) {
                for (int32_t z = 0; z <= 16; ++z) {
                    it->second.setBlock(x, 0, z, -567203660);
                }
            }
        }

        return &it->second;
    }

    std::string ChunkGenerator::getChunkKey(const int32_t chunkX, const int32_t chunkY) {
        return std::format("{};{}", chunkX, chunkY);
    }
}
