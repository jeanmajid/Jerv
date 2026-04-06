#pragma once
#include "jerv/raknet/protocol/raknetBasePacket.hpp"

namespace jerv::raknet {
    class ConnectionRequestAcceptedPacket : public RaknetBasePacket {
    public:
        binary::Address clientAddress;
        int16_t systemIndex;
        std::vector<binary::Address> systemAddresses{10};
        int64_t pingTime;
        int64_t pongTime;

        RaknetPacketId getPacketId() const override {
            return RaknetPacketId::ConnectionRequestAccepted;
        }

        void serialize(binary::Cursor &cursor) const override {
            cursor.writeAddress(clientAddress);
            cursor.writeInt16(systemIndex);
            for (const binary::Address& address : systemAddresses) {
                cursor.writeAddress(address);
            }

            cursor.writeInt64(pingTime);
            cursor.writeInt64(pongTime);
        }

        void deserialize(binary::Cursor &cursor) override {
            (void) cursor;
        }
    };
}
