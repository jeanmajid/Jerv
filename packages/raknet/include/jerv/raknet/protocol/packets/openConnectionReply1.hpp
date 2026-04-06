#pragma once
#include "jerv/raknet/protocol/raknetBasePacket.hpp"

namespace jerv::raknet {
    class OpenConnectionReply1Packet : public RaknetBasePacket {
    public:
        int64_t serverGuid;
        bool serverHasSecurity;
        uint32_t cookie;
        uint16_t mtuSize;

        RaknetPacketId getPacketId() const override {
            return RaknetPacketId::OpenConnectionReply1;
        }

        void serialize(binary::Cursor &cursor) const override {
            cursor.writeMagic();
            cursor.writeInt64(serverGuid);
            cursor.writeBool(serverHasSecurity);
            if (serverHasSecurity) {
                cursor.writeUint32(cookie);
            }

            cursor.writeUint16(mtuSize);
        }

        void deserialize(binary::Cursor &cursor) override {
            (void) cursor;
        }
    };
}
