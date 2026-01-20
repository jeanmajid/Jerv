#pragma once

#include <jerv/protocol/packet.hpp>
#include <jerv/protocol/enums.hpp>

namespace jerv::protocol {
    class PlayerAuthInputPacket : public PacketType {
    public:
        // TODO: rest of the packet not implemented yet, obviously for perfomance reasons, not bcs im lazy :)
        float pitch;
        float yaw;
        Vec3f position;

        PacketId getPacketId() const override {
            return PacketId::PlayerAuthInput;
        }

        void serialize(binary::cursor &cursor) const override {
        }

        void deserialize(binary::cursor &cursor) override {
            pitch = cursor.readFloat32<true>();
            yaw = cursor.readFloat32<true>();
            position.deserialize(cursor);
        }
    };
}
