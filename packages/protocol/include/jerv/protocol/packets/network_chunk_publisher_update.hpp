#pragma once

#include <jerv/protocol/packet.hpp>
#include <jerv/protocol/enums.hpp>
#include <vector>
#include <cstdint>

namespace jerv::protocol {
    class NetworkChunkPublisherUpdatePacket : public PacketType {
    public:
        BlockLocation coordinate;
        uint32_t radius = 0;
        std::vector<ChunkCoords> savedChunks;

        PacketId getPacketId() const override {
            return PacketId::NetworkChunkPublisherUpdate;
        }

        void serialize(binary::cursor &cursor) const override {
            coordinate.serialize(cursor);
            cursor.writeVarInt(static_cast<int32_t>(radius));
            cursor.writeUint32(static_cast<uint32_t>(savedChunks.size()), true);

            for (const auto &chunk: savedChunks) {
                chunk.serialize(cursor);
            }
        }

        void deserialize(binary::cursor &cursor) override {
            coordinate.deserialize(cursor);
            radius = static_cast<uint32_t>(cursor.readVarInt());
            const uint32_t count = cursor.readUint32(true);
            savedChunks.clear();
            savedChunks.reserve(count);

            for (uint32_t i = 0; i < count; i++) {
                ChunkCoords coords;
                coords.deserialize(cursor);
                savedChunks.push_back(coords);
            }
        }
    };
}
