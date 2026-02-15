#pragma once

#include <jerv/protocol/packet.hpp>
#include <jerv/protocol/enums.hpp>
#include <vector>
#include <cstdint>

namespace jerv::protocol {
    class SubChunkRequestPacket : public PacketType {
    public:
        struct RequestEntry {
            ChunkCoords coords;
            uint8_t minIndex = 0;
            uint8_t maxIndex = 0;

            void serialize(binary::Cursor &cursor) const {
                coords.serialize(cursor);
                cursor.writeUint8(minIndex);
                cursor.writeUint8(maxIndex);
            }

            void deserialize(binary::Cursor &cursor) {
                coords.deserialize(cursor);
                minIndex = cursor.readUint8();
                maxIndex = cursor.readUint8();
            }
        };

        DimensionId dimension = DimensionId::Overworld;
        std::vector<RequestEntry> requests;

        PacketId getPacketId() const override { return PacketId::SubChunkRequest; }

        void serialize(binary::Cursor &cursor) const override {
            cursor.writeZigZag32(static_cast<int32_t>(dimension));
            cursor.writeVarInt32(static_cast<int32_t>(requests.size()));
            for (const auto &req : requests) {
                req.serialize(cursor);
            }
        }

        void deserialize(binary::Cursor &cursor) override {
            dimension = static_cast<DimensionId>(cursor.readZigZag32());
            const int32_t count = cursor.readVarInt32();
            requests.clear();
            requests.reserve(count);
            for (int32_t i = 0; i < count; i++) {
                RequestEntry entry;
                entry.deserialize(cursor);
                requests.push_back(entry);
            }
        }
    };
}
