#pragma once
#include "jerv/raknet/protocol/raknetBasePacket.hpp"

namespace jerv::raknet {
    class UnconnectPingPacket : public RaknetBasePacket {
    public:
        uint64_t clientAliveTimeMs;
        int64_t clientGuid;

        RaknetPacketId getPacketId() const override {
            return RaknetPacketId::UnconnectPing;
        }

        void serialize(jerv::binary::Cursor &cursor) const override {
        }

        void deserialize(binary::Cursor &cursor) override {
            clientAliveTimeMs = cursor.readUint64();
            cursor.readMagic();
            clientGuid = cursor.readInt64();
        }
    };
}
