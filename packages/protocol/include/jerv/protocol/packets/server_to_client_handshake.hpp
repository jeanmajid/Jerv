#pragma once

#include <jerv/protocol/packet.hpp>
#include <string>

namespace jerv::protocol {
    class ServerToClientHandshakePacket : public PacketType {
    public:
        std::string token;

        PacketId getPacketId() const override {
            return PacketId::ServerToClientHandshake;
        }

        void serialize(binary::cursor &cursor) const override {
            cursor.writeString(token);
        }

        void deserialize(binary::cursor &cursor) override {
            token = cursor.readString();
        }
    };
}