#pragma once

#include <jerv/protocol/packet.hpp>

namespace jerv::protocol {
    class SetTimePacket : public PacketType {
    public:
        int32_t time;

        PacketId getPacketId() const override {
            return PacketId::SetTime;
        }

        void serialize(binary::Cursor &cursor) const override {
            cursor.writeVarInt32(time);
        }

        void deserialize(binary::Cursor &cursor) override {
            time = cursor.readVarInt32();
        }
    };
}