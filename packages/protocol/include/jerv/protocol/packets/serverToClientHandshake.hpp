#pragma once

#include <jerv/protocol/packet.hpp>
#include <string>

namespace jerv::protocol {
    class ServerToClientHandshakePacket : public PacketType {
    public:
        std::string token;

        static constexpr auto ID = PacketId::ServerToClientHandshake;
        PacketId getPacketId() const override {
            return PacketId::ServerToClientHandshake;
        }

        void serialize(binary::Cursor &cursor) const override {
            cursor.writeString(token);
        }

        void deserialize(binary::Cursor &cursor) override {
            token = cursor.readString();
        }
    };
}