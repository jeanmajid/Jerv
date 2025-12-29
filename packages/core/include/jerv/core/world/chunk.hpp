#pragma once

#include <jerv/core/world/sub_chunk.hpp>
#include <jerv/binary/cursor.hpp>
#include <jerv/protocol/enums.hpp>
#include <jerv/protocol/packets/sub_chunk.hpp>
#include <vector>
#include <cstdint>
#include <memory>
#include <optional>

namespace jerv::core {
    namespace BlockIds {
        constexpr int32_t Air = -604749536;
        constexpr int32_t Stone = -2144268767;
        constexpr int32_t GrassBlock = -567203660;
        constexpr int32_t Dirt = -2108756090;
        constexpr int32_t Cobblestone = -302570548;
        constexpr int32_t Bedrock = -173245189;
        constexpr int32_t Water = 1211861802;
        constexpr int32_t Sand = 138639715;
        constexpr int32_t Glowstone = -2040923292;
    }

    class Chunk {
    public:
        static constexpr int32_t MAX_SUB_CHUNKS = 24;
        static constexpr int32_t OVERWORLD_MIN_Y = -64;
        static constexpr int32_t OVERWORLD_MAX_Y = 319;
        static constexpr int32_t NETHER_MIN_Y = 0;
        static constexpr int32_t NETHER_MAX_Y = 127;
        static constexpr int32_t END_MIN_Y = 0;
        static constexpr int32_t END_MAX_Y = 255;

        Chunk(const int32_t x, const int32_t z,
              const protocol::DimensionId dimension = protocol::DimensionId::Overworld)
            : x_(x), z_(z), dimension_(dimension), dirty_(false) {
            subchunks_.resize(MAX_SUB_CHUNKS);
        }

        int32_t x() const { return x_; }
        int32_t z() const { return z_; }
        protocol::DimensionId dimension() const { return dimension_; }
        bool isDirty() const { return dirty_; }

        void setDirty(const bool dirty = true) {
            dirty_ = dirty;
            cache_.reset();
        }

        int32_t minY() const {
            switch (dimension_) {
                case protocol::DimensionId::Overworld: return OVERWORLD_MIN_Y;
                case protocol::DimensionId::Nether: return NETHER_MIN_Y;
                case protocol::DimensionId::End: return END_MIN_Y;
                default: return 0;
            }
        }

        int32_t maxY() const {
            switch (dimension_) {
                case protocol::DimensionId::Overworld: return OVERWORLD_MAX_Y;
                case protocol::DimensionId::Nether: return NETHER_MAX_Y;
                case protocol::DimensionId::End: return END_MAX_Y;
                default: return 255;
            }
        }

        int32_t yToSubChunkIndex(const int32_t y) const {
            const int32_t offset = (dimension_ == protocol::DimensionId::Overworld) ? 4 : 0;
            return (y >> 4) + offset;
        }

        SubChunk &getSubChunk(const int32_t index) {
            if (index < 0 || index >= MAX_SUB_CHUNKS) {
                static SubChunk empty;
                return empty;
            }
            if (!subchunks_[index]) {
                int8_t subIndex = static_cast<int8_t>(index);
                if (dimension_ == protocol::DimensionId::Overworld) {
                    subIndex -= 4;
                }
                subchunks_[index] = std::make_unique<SubChunk>(subIndex);
            }
            return *subchunks_[index];
        }

        const SubChunk *getSubChunk(const int32_t index) const {
            if (index < 0 || index >= MAX_SUB_CHUNKS) return nullptr;
            return subchunks_[index].get();
        }

        int32_t getBlock(const int32_t x, const int32_t y, const int32_t z, const size_t layer = 0) const {
            const int32_t index = yToSubChunkIndex(y);
            const SubChunk *sub = getSubChunk(index);
            if (!sub) return BlockIds::Air;
            return sub->getState(x & 0xF, y & 0xF, z & 0xF, layer);
        }

        void setBlock(const int32_t x, const int32_t y, const int32_t z, const int32_t state, const size_t layer = 0) {
            const int32_t index = yToSubChunkIndex(y);
            getSubChunk(index).setState(x & 0xF, y & 0xF, z & 0xF, state, layer);
            setDirty();
        }

        void fill(const int32_t x1, const int32_t y1, const int32_t z1,
                  const int32_t x2, const int32_t y2, const int32_t z2,
                  const int32_t state, const size_t layer = 0) {
            for (int32_t y = y1; y <= y2; y++) {
                for (int32_t x = x1; x <= x2; x++) {
                    for (int32_t z = z1; z <= z2; z++) {
                        setBlock(x, y, z, state, layer);
                    }
                }
            }
        }

        int32_t getSubChunkSendCount() const {
            int32_t count = 0;
            for (int32_t i = MAX_SUB_CHUNKS - 1; i >= 0; i--) {
                if (subchunks_[i] && !subchunks_[i]->isEmpty()) {
                    count = i + 1;
                    break;
                }
            }
            return count;
        }

        std::vector<uint8_t> serialize() {
            if (cache_) return *cache_;

            binary::ResizableCursor cursor(4096, 65536);
            const int32_t subChunkCount = getSubChunkSendCount();
            const int32_t offset = (dimension_ == protocol::DimensionId::Overworld) ? 4 : 0;

            for (int32_t i = 0; i < subChunkCount; i++) {
                if (subchunks_[i]) {
                    subchunks_[i]->serialize(cursor);
                } else {
                    SubChunk empty(static_cast<int8_t>(i - offset));
                    empty.serialize(cursor);
                }
            }

            for (int32_t i = 0; i < subChunkCount; i++) {
                cursor.growToFit(5);
                cursor.writeUint8(0);
                cursor.writeUint32(1, true);
            }

            cursor.writeUint8(0);

            auto data = cursor.getProcessedBytes();
            cache_ = std::vector(data.begin(), data.end());
            return *cache_;
        }


        std::vector<protocol::SubChunkPacket> serializeSubChunks() const {
            std::vector<protocol::SubChunkPacket> packets;
            const int32_t subChunkCount = getSubChunkSendCount();
            const int32_t offset = (dimension_ == protocol::DimensionId::Overworld) ? 4 : 0;

            packets.reserve(subChunkCount);
            for (int32_t i = 0; i < subChunkCount; i++) {
                const SubChunk *sub = subchunks_[i].get();
                binary::ResizableCursor cursor(256, 4096);
                if (sub) {
                    sub->serialize(cursor);
                } else {
                    SubChunk empty(static_cast<int8_t>(i - offset));
                    empty.serialize(cursor);
                }

                protocol::SubChunkPacket packet;
                packet.x = x_;
                packet.z = z_;
                packet.dimension = dimension_;
                packet.subChunkIndex = static_cast<int8_t>(i - offset);
                packet.cacheEnabled = false;
                auto bytes = cursor.getProcessedBytes();
                packet.data.assign(bytes.begin(), bytes.end());
                packets.push_back(std::move(packet));
            }
            return packets;
        }


        void generateFlatGrass(const int32_t groundLevel = 64) {
            const int32_t bedrockY = minY();
            const int32_t dirtStartY = bedrockY + 1;
            const int32_t dirtEndY = groundLevel - 1;
            const int32_t grassY = groundLevel;

            for (int32_t x = 0; x < 16; x++) {
                for (int32_t z = 0; z < 16; z++) {
                    setBlock(x, bedrockY, z, BlockIds::Bedrock);
                }
            }

            for (int32_t y = dirtStartY; y <= dirtEndY; y++) {
                for (int32_t x = 0; x < 16; x++) {
                    for (int32_t z = 0; z < 16; z++) {
                        setBlock(x, y, z, BlockIds::Dirt);
                    }
                }
            }

            for (int32_t x = 0; x < 16; x++) {
                for (int32_t z = 0; z < 16; z++) {
                    setBlock(x, grassY, z, BlockIds::GrassBlock);
                }
            }
            setDirty();
        }

        void generateStone(const int32_t height = 64) {
            const int32_t bedrockY = minY();

            for (int32_t y = bedrockY; y <= height; y++) {
                const int32_t state = (y == bedrockY) ? BlockIds::Bedrock : BlockIds::Stone;
                for (int32_t x = 0; x < 16; x++) {
                    for (int32_t z = 0; z < 16; z++) {
                        setBlock(x, y, z, state);
                    }
                }
            }
            setDirty();
        }

        void clear() {
            for (auto &sub: subchunks_) {
                sub.reset();
            }
            setDirty();
        }

    private:
        int32_t x_;
        int32_t z_;
        protocol::DimensionId dimension_;
        bool dirty_;
        std::vector<std::unique_ptr<SubChunk> > subchunks_;
        std::optional<std::vector<uint8_t> > cache_;
    };
}
