#pragma once

#include <jerv/protocol/packet.hpp>
#include <jerv/protocol/enums.hpp>
#include <vector>
#include <cstdint>

namespace jerv::protocol {
    class SubChunkPacket : public PacketType {
    public:
        int32_t x = 0;
        int32_t z = 0;
        DimensionId dimension = DimensionId::Overworld;
        int8_t subChunkIndex = 0;
        bool cacheEnabled = false;
        std::vector<uint64_t> blobs;
        std::vector<uint8_t> data;

        PacketId getPacketId() const override { return PacketId::SubChunk; }

        void serialize(binary::cursor &cursor) const override {
            cursor.writeZigZag32(x);
            cursor.writeZigZag32(z);
            cursor.writeZigZag32(static_cast<int32_t>(dimension));
            cursor.writeUint8(static_cast<uint8_t>(subChunkIndex));
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

        void deserialize(binary::cursor &cursor) override {
            x = cursor.readZigZag32();
            z = cursor.readZigZag32();
            dimension = static_cast<DimensionId>(cursor.readZigZag32());
            subChunkIndex = static_cast<int8_t>(cursor.readUint8());
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
