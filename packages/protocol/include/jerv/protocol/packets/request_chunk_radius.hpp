#pragma once

#include <jerv/protocol/packet.hpp>

namespace jerv::protocol {
    class RequestChunkRadiusPacket : public PacketType {
    public:
        int32_t chunkRadius;
        uint8_t maxChunkRadius;

        PacketId getPacketId() const override {
            return PacketId::RequestChunkRadius;
        }

        void serialize(binary::Cursor &cursor) const override {
        }

        void deserialize(binary::Cursor &cursor) override {
            chunkRadius = cursor.readZigZag32();
            maxChunkRadius = cursor.readUint8();
        }
    };
}
