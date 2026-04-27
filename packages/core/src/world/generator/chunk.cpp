#include "jerv/core/world/generator/chunk.hpp"

namespace jerv::core::world::generator {
    int32_t Chunk::getBlock(const int32_t x, const int32_t y, const int32_t z, const size_t layer) {
        const int32_t index = yToSubChunkIndex(y);
        SubChunk *sub = getSubChunkOptional(index);
        if (!sub) {
            return -604749536; // AIR TODO: replace with some actual block getter generation logic
        }
        return sub->getState(x & 0xF, y & 0xF, z & 0xF, layer);
    }

    void Chunk::setBlock(const int32_t x, const int32_t y, const int32_t z, const int32_t state, const size_t layer) {
        const int32_t index = yToSubChunkIndex(y);
        getSubChunk(index).setState(x & 0xF, y & 0xF, z & 0xF, state, layer);
        // setDirty();
    }

    protocol::LevelChunkPacket Chunk::serialize() {
        if (cache) return *cache;

        binary::ResizableCursor cursor(4096, 65536);
        const int32_t subChunkCount = getSubChunkSendCount();

        for (int32_t i = 0; i < subChunkCount; i++) {
            if (subchunks[i]) {
                subchunks[i]->serialize(cursor);
            } else {
                SubChunk empty;
                empty.serialize(cursor);
            }
        }

        for (int32_t i = 0; i < subChunkCount; i++) {
            cursor.growToFit(5);
            cursor.writeUint8(0);
            cursor.writeUint32<true>(1);
        }

        cursor.writeUint8(0);

        auto data = cursor.getProcessedBytes();

        protocol::LevelChunkPacket levelChunkPacket;
        levelChunkPacket.x = chunkX;
        levelChunkPacket.z = chunkZ;
        levelChunkPacket.dimension = dimension;
        levelChunkPacket.subChunkCount = subChunkCount;
        levelChunkPacket.data = std::vector(data.begin(), data.end());

        cache = levelChunkPacket;
        return *cache;
    }

    int32_t Chunk::getSubChunkSendCount() {
        int32_t count = 0;
        for (int32_t i = MAX_SUB_CHUNKS - 1; i >= 0; i--) {
            if (subchunks[i] && !subchunks[i]->isEmpty()) {
                count = i + 1;
                break;
            }
        }
        return count;
    }

    int32_t Chunk::yToSubChunkIndex(const int32_t y) {
        const int32_t offset = (dimension == protocol::DimensionId::Overworld) ? 4 : 0;
        return (y >> 4) + offset;
    }

    SubChunk *Chunk::getSubChunkOptional(const int32_t index) {
        if (index < 0 || index >= MAX_SUB_CHUNKS) return nullptr;
        return subchunks[index].get();
    }

    SubChunk & Chunk::getSubChunk(const int32_t index) {
        if (index < 0 || index >= MAX_SUB_CHUNKS) {
            static SubChunk empty;
            return empty;
        }
        if (!subchunks[index]) {
            subchunks[index] = std::make_unique<SubChunk>();
        }
        return *subchunks[index];
    }
}
