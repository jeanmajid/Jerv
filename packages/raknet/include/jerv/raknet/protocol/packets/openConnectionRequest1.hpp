#pragma once
#include "jerv/raknet/protocol/raknetBasePacket.hpp"

namespace jerv::raknet {
    class OpenConnectionRequest1Packet : public RaknetBasePacket {
    public:
        uint8_t protocolVersion;

        RaknetPacketId getPacketId() const override {
            return RaknetPacketId::OpenConnectionRequest1;
        }

        void serialize(binary::Cursor &cursor) const override {
            (void) cursor;
        }

        void deserialize(binary::Cursor &cursor) override {
            cursor.readMagic();
            protocolVersion = cursor.readUint8();
            // TODO: Figure out MTU based on the padding here
        }
    };
}
