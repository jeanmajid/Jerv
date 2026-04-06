#pragma once
#include "jerv/raknet/protocol/raknetBasePacket.hpp"

namespace jerv::raknet {
    class NewIncomingConnectionPacket : public RaknetBasePacket {
    public:
        uint8_t serverAddress;
        std::vector<binary::Address> clientMachineAddresses{10};
        uint64_t clientSendTime;
        uint64_t serverSendTime;

        RaknetPacketId getPacketId() const override {
            return RaknetPacketId::NewIncomingConnection;
        }

        void serialize(jerv::binary::Cursor &cursor) const override {
            (void) cursor;
        }

        void deserialize(binary::Cursor &cursor) override {
            (void) cursor;
        }
    };
}
