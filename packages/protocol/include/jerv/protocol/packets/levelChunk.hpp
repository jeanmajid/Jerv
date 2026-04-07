#pragma once

#include <jerv/protocol/packet.hpp>
#include <jerv/protocol/enums.hpp>
#include <vector>
#include <cstdint>

namespace jerv::protocol {
    class LevelChunkPacket : public PacketType {
    public:
        int32_t x = 0;
        int32_t z = 0;
        DimensionId dimension = DimensionId::Overworld;

        int32_t subChunkCount = 0;
        uint16_t highestSubChunkCount = 0;

        bool cacheEnabled = false;
        std::vector<uint64_t> blobs;
        std::vector<uint8_t> data;

        PacketId getPacketId() const override {
            return PacketId::LevelChunk;
        }

        void serialize(binary::Cursor &cursor) const override {
            cursor.writeZigZag32(x);
            cursor.writeZigZag32(z);
            cursor.writeZigZag32(static_cast<int32_t>(dimension));
            cursor.writeVarInt32(subChunkCount);

            if (subChunkCount == -2) {
                cursor.writeUint16<true>(highestSubChunkCount);
            }

            cursor.writeBool(cacheEnabled);

            if (cacheEnabled) {
                cursor.writeVarInt32(static_cast<int32_t>(blobs.size()));
                for (uint64_t hash: blobs) {
                    cursor.writeUint64<true>(hash);
                }
            }

            cursor.writeVarInt32(static_cast<int32_t>(data.size()));
            cursor.writeSliceSpan(data);
        }

        void deserialize(binary::Cursor &cursor) override {
            x = cursor.readZigZag32();
            z = cursor.readZigZag32();
            dimension = static_cast<DimensionId>(cursor.readZigZag32());
            subChunkCount = cursor.readVarInt32();

            if (subChunkCount == -2 || subChunkCount == static_cast<int32_t>(0xFFFFFFFE)) {
                subChunkCount = -2;
                highestSubChunkCount = cursor.readUint16<true>();
            }

            cacheEnabled = cursor.readBool();

            blobs.clear();
            if (cacheEnabled) {
                const int32_t blobCount = cursor.readVarInt32();
                if (blobCount > 64) {
                    throw std::runtime_error("Too many blob hashes");
                }
                blobs.reserve(blobCount);
                for (int32_t i = 0; i < blobCount; i++) {
                    blobs.push_back(cursor.readUint64<true>());
                }
            }

            const int32_t dataLength = cursor.readVarInt32();
            auto dataSpan = cursor.readSliceSpan(static_cast<size_t>(dataLength));
            data.assign(dataSpan.begin(), dataSpan.end());
        }
    };
}