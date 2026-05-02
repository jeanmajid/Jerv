#include "jerv/core/world/generator/generator.hpp"

namespace jerv::core::world::generator {
    struct ChunkOffset {
        int32_t dx, dz;
        int32_t distSq;

        bool operator<(const ChunkOffset &other) const {
            return distSq < other.distSq;
        }
    };

    std::pair<std::vector<protocol::ChunkCoords>, std::vector<Chunk *> > ChunkGenerator::generateChunks(
        const protocol::Vec3f location, const int32_t chunkRadius) {
        const int32_t centerChunkX = static_cast<int32_t>(std::floor(location.x / 16.0f));
        const int32_t centerChunkZ = static_cast<int32_t>(std::floor(location.z / 16.0f));

        const int32_t radiusSq = chunkRadius * chunkRadius;
        std::vector<ChunkOffset> offsets;
        offsets.reserve(3.14159f * radiusSq);

        for (int32_t dx = -chunkRadius; dx <= chunkRadius; ++dx) {
            for (int32_t dz = -chunkRadius; dz <= chunkRadius; ++dz) {
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

        for (const auto &offset: offsets) {
            if (chunksSend >= maxChunksToSend) {
                break;
            }

            int32_t chunkX = centerChunkX + offset.dx;
            int32_t chunkZ = centerChunkZ + offset.dz;

            Chunk *chunk = generateChunk(chunkX, chunkZ);
            if (chunk->hasBeenSent) {
                continue;
            }

            chunk->hasBeenSent = true;
            generatedChunks.emplace_back(chunk);
            coords.emplace_back(chunkX, chunkZ);
            ++chunksSend;
        }

        return {std::move(coords), std::move(generatedChunks)};
    }

    Chunk *ChunkGenerator::generateChunk(const int32_t chunkX, const int32_t chunkZ) {
        const auto [it, inserted] = chunks.try_emplace(getChunkKey(chunkX, chunkZ), chunkX, chunkZ);
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

    std::string ChunkGenerator::getChunkKey(const int32_t chunkX, const int32_t chunkY) {
        return std::format("{};{}", chunkX, chunkY);
    }
}
