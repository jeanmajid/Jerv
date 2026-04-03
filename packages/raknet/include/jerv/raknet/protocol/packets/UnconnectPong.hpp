#pragma once
#include "jerv/raknet/protocol/RaknetBasePacket.hpp"

namespace jerv::raknet {
    class UnconnectPongPacket : public RaknetBasePacket {
    public:
        uint64_t clientAliveTimeMs;
        int64_t serverGuid;
        std::string motd;

        RaknetPacketId getPacketId() const override {
            return RaknetPacketId::UnconnectPong;
        }

        void serialize(binary::Cursor &cursor) const override {
            cursor.writeUint64(clientAliveTimeMs);
            cursor.writeInt64(serverGuid);
            cursor.writeMagic();
            cursor.writeUint16(motd.length());
            cursor.writeSliceSpan(std::span(
                reinterpret_cast<const uint8_t *>(motd.data()),
                motd.size()
            ));
        }

        void deserialize(binary::Cursor &cursor) override {
        }
    };
}
