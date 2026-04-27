#include "jerv/core/world/generator/generator.hpp"

namespace jerv::core::world::generator {
    std::pair<std::vector<protocol::ChunkCoords>, std::vector<Chunk *> > ChunkGenerator::generateChunks(
        const protocol::Vec3f location, const int32_t chunkRadius) {
        const int32_t startX = std::floor((location.x - chunkRadius) / 16);
        const int32_t endX = std::floor((location.x + chunkRadius) / 16);
        const int32_t startZ = std::floor((location.z - chunkRadius) / 16);
        const int32_t endZ = std::floor((location.z + chunkRadius) / 16);

        const size_t total = (endX - startX + 1) * (endZ - startZ + 1);

        std::vector<Chunk *> generatedChunks;
        std::vector<protocol::ChunkCoords> coords;
        generatedChunks.reserve(total);
        coords.reserve(total);

        // TODO: on a render distance of 95 this sends 36481 chunks all at once
        // TODO: the client does not like this
        // TODO: send chunks closest to furthest (I think this fixes the chunks not being properly accepted by the client)
        // TODO: and send chunks gradually on an set limit to not blow up the server and client

        for (int32_t chunkX = startX; chunkX <= endX; ++chunkX) {
            for (int32_t chunkZ = startZ; chunkZ <= endZ; ++chunkZ) {
                Chunk* chunk = generateChunk(chunkX, chunkZ);
                if (chunk->hasBeenSent) {
                    continue;
                }
                chunk->hasBeenSent = true;
                generatedChunks.emplace_back(chunk);
                coords.emplace_back(chunkX, chunkZ);
            }
        }

        return {std::move(coords), std::move(generatedChunks)};
    }

    Chunk *ChunkGenerator::generateChunk(const int32_t chunkX, const int32_t chunkZ) {
        const auto [it, inserted] = chunks.try_emplace(getChunkKey(chunkX, chunkZ), chunkX, chunkZ);
        if (inserted) {
            it->second.setBlock(0, 0, 0, -567203660);
        }

        return &it->second;
    }

    std::string ChunkGenerator::getChunkKey(const int32_t chunkX, const int32_t chunkY) {
        return std::format("{};{}", chunkX, chunkY);
    }
}
