#pragma once

#include <jerv/protocol/packet.hpp>
#include <vector>
#include <string>

namespace jerv::protocol {
    enum class LoadingScreenType : uint32_t {
        StartLoadingScreen = 0,
        EndLoadingScreen = 1,
    };

    class ServerBoundLoadingScreenPacket : public PacketType {
    public:
        LoadingScreenType loadingScreenType;
        uint32_t loadingScreenId;

        static constexpr auto ID = PacketId::ServerboundLoadingScreen;
        PacketId getPacketId() const override {
            return PacketId::AvailableActorIdentifiers;
        }

        void serialize(binary::Cursor &cursor) const override {
        }

        void deserialize(binary::Cursor &cursor) override {
            loadingScreenType = static_cast<LoadingScreenType>(cursor.readVarInt32());
            loadingScreenId = cursor.readUint32();
        }
    };
}
