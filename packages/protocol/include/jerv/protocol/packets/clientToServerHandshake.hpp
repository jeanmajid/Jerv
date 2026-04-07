#pragma once

#include <jerv/protocol/packet.hpp>

namespace jerv::protocol {
    class ClientToServerHandshakePacket : public PacketType {
    public:
        PacketId getPacketId() const override {
            return PacketId::ClientToServerHandshake;
        }

        void serialize(binary::Cursor &) const override {
        }

        void deserialize(binary::Cursor &) override {
        }
    };
}