#pragma once

#include <jerv/protocol/packet.hpp>

namespace jerv::protocol {
    class SetTimePacket : public PacketType {
    public:
        int32_t time;

        PacketId getPacketId() const override {
            return PacketId::SetTime;
        }

        void serialize(binary::cursor &cursor) const override {
            cursor.writeVarInt(time);
        }

        void deserialize(binary::cursor &cursor) override {
            time = cursor.readVarInt();
        }
    };
}