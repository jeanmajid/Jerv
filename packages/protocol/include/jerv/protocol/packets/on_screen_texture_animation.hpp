#pragma once

#include <jerv/protocol/packet.hpp>

namespace jerv::protocol {
    class OnScreenTextureAnimationPacket : public PacketType {
    public:
        uint32_t effectId;

        PacketId getPacketId() const override {
            return PacketId::OnScreenTextureAnimation;
        }

        void serialize(binary::Cursor &cursor) const override {
            cursor.writeUint32(effectId);
        }

        void deserialize(binary::Cursor &cursor) override {
            effectId = cursor.readUint32();
        }
    };
}