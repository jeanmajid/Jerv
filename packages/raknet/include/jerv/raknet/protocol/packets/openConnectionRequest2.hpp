#pragma once
#include "jerv/raknet/protocol/raknetBasePacket.hpp"

namespace jerv::raknet {
    class OpenConnectionRequest2Packet : public RaknetBasePacket {
    public:
        binary::Address serverAddress;
        uint16_t mtuSize;
        int64_t clientGuid;

        RaknetPacketId getPacketId() const override {
            return RaknetPacketId::OpenConnectionRequest2;
        }

        void serialize(binary::Cursor &cursor) const override {
            (void) cursor;
        }

        void deserialize(binary::Cursor &cursor) override {
            cursor.readMagic();
            serverAddress = cursor.readAddress();
            mtuSize = cursor.readUint16();
            clientGuid = cursor.readInt64();
        }
    };
}
