#pragma once

#include <jerv/protocol/packet.hpp>

namespace jerv::protocol {
    class ChunkRadiusUpdatePacket : public PacketType {
    public:
        int32_t radius = 4;

        PacketId getPacketId() const override {
            return PacketId::ChunkRadiusUpdated;
        }

        void serialize(binary::Cursor &cursor) const override {
            cursor.writeZigZag32(radius);
        }

        void deserialize(binary::Cursor &cursor) override {
            radius = cursor.readZigZag32();
        }
    };
}