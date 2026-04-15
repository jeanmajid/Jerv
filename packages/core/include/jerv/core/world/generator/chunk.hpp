#pragma once
#include <cstdint>

#include "subChunk.hpp"
#include "jerv/protocol/enums.hpp"
#include "jerv/protocol/packets/levelChunk.hpp"

namespace jerv::protocol {
    enum class DimensionId;
}

namespace jerv::core::world {
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

    private:
        int32_t getSubChunkSendCount();

        int32_t yToSubChunkIndex(int32_t y);

        SubChunk *getSubChunkOptional(int32_t index);

        SubChunk &getSubChunk(int32_t index);

        std::vector<std::unique_ptr<SubChunk> > subchunks;

        int32_t chunkX;
        int32_t chunkZ;
        protocol::DimensionId dimension;

        int32_t minY;
        int32_t maxY;

        std::optional<protocol::LevelChunkPacket> cache;
    };
}
