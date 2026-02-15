#pragma once

#include <jerv/protocol/packet.hpp>

namespace jerv::protocol {
    class ClientboundDataDrivenUIShowScreenPacket : public PacketType {
    public:
        std::string screenId;

        PacketId getPacketId() const override {
            return PacketId::ClientboundDataDrivenUIShowScreenPacket;
        }

        void serialize(binary::Cursor &cursor) const override {
            cursor.writeString(screenId);
        }

        void deserialize(binary::Cursor &cursor) override {

        }
    };
}
