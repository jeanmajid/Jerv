#pragma once

#include <jerv/protocol/packet.hpp>

namespace jerv::protocol {
    class RequestNetworkSettingsPacket : public PacketType {
    public:
        int32_t clientNetworkVersion = 0;

        PacketId getPacketId() const override {
            return PacketId::RequestNetworkSettings;
        }

        void serialize(binary::cursor &cursor) const override {
            cursor.writeUint32(static_cast<uint32_t>(clientNetworkVersion));
        }

        void deserialize(binary::cursor &cursor) override {
            clientNetworkVersion = static_cast<int32_t>(cursor.readUint32());
        }
    };
}