#pragma once

#include <jerv/protocol/packet.hpp>
#include <jerv/protocol/enums.hpp>

namespace jerv::protocol {
    class InteractPacket : public PacketType {
    public:
        // TODO
        uint8_t actionId;
        uint64_t targetEntityId;

        PacketId getPacketId() const override {
            return PacketId::Interact;
        }

        void serialize(binary::cursor &cursor) const override {
        }

        void deserialize(binary::cursor &cursor) override {
            actionId = cursor.readUint8();
            targetEntityId = cursor.readVarInt();
        }
    };
}
