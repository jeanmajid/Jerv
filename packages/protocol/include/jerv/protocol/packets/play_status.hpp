#pragma once

#include <jerv/protocol/packet.hpp>

namespace jerv::protocol {
    enum class PlayStatus : int32_t {
        LoginSuccess = 0,
        FailedClient = 1,
        FailedServer = 2,
        PlayerSpawn = 3,
        FailedInvalidTenant = 4,
        FailedVanillaEdu = 5,
        FailedIncompatible = 6,
        FailedServerFull = 7,
        FailedEditorVanillaMismatch = 8,
        FailedVanillaEditorMismatch = 9
    };

    class playStatusPacket : public PacketType {
    public:
        PlayStatus status = PlayStatus::LoginSuccess;

        PacketId getPacketId() const override {
            return PacketId::PlayStatus;
        }

        void serialize(binary::cursor &cursor) const override {
            cursor.writeInt32(static_cast<int32_t>(status));
        }

        void deserialize(binary::cursor &cursor) override {
            status = static_cast<PlayStatus>(cursor.readInt32());
        }
    };
}