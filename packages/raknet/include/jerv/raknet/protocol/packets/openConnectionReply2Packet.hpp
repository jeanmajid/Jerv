#pragma once
#include "jerv/raknet/protocol/raknetBasePacket.hpp"

namespace jerv::raknet {
    class OpenConnectionReply2 : public RaknetBasePacket {
    public:
        int64_t serverGuid;
        binary::Address clientAddress;
        uint16_t mtuSize;
        bool security;

        RaknetPacketId getPacketId() const override {
            return RaknetPacketId::OpenConnectionReply2;
        }

        void serialize(binary::Cursor &cursor) const override {
            cursor.writeMagic();
            cursor.writeInt64(serverGuid);
            cursor.writeAddress(clientAddress);
            cursor.writeUint16(mtuSize);
            cursor.writeBool(security);
        }

        void deserialize(binary::Cursor &cursor) override {

        }
    };
}
