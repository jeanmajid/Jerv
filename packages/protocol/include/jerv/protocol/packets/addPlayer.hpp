#pragma once

#include <jerv/protocol/packet.hpp>

namespace jerv::protocol {
    class AddPlayerPacket : public PacketType {
    public:
        static constexpr auto ID = PacketId::AddPlayer;
        PacketId getPacketId() const override {
            return PacketId::AddPlayer;
        }
        
        void serialize(binary::Cursor &cursor) const override {

        }

        void deserialize(binary::Cursor &cursor) override {
        }
    };
}