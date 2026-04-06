#pragma once
#include "jerv/raknet/protocol/raknetBasePacket.hpp"

namespace jerv::raknet {
    class ConnectionRequestPacket : public RaknetBasePacket {
    public:
        int64_t clientGuid;
        int64_t requestTimestamp;
        bool secure;

        RaknetPacketId getPacketId() const override {
            return RaknetPacketId::ConnectionRequest;
        }

        void serialize(binary::Cursor &cursor) const override {
        }

        void deserialize(binary::Cursor &cursor) override {
            clientGuid = cursor.readInt64();
            requestTimestamp = cursor.readInt64();
            secure = cursor.readBool();
        }
    };
}
