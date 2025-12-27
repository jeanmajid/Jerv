#pragma once

#include <jerv/raknet/base_connection.hpp>
#include <functional>

namespace jerv::raknet {
    class ServerConnection : public BaseConnection {
    public:
        ServerConnection(
            SocketSource &source,
            const AddressInfo &endpoint,
            const AddressInfo &serverAddress,
            uint64_t guid,
            size_t mtuSize
        );

        void disconnect() override;

        std::function<void()> onDisconnect;
        std::function<void(std::span<uint8_t>)> onGamePacket;

    private:
        void onFrameReceived(FrameDescriptor &desc);
        void handleConnectionRequest(std::span<uint8_t> message);
        void handleDisconnect(std::span<uint8_t> message);
        void handleConnectedPing(std::span<uint8_t> message);
        void handleNewIncomingConnection(std::span<uint8_t> message);
        void handleGameData(std::span<uint8_t> message);

        AddressInfo serverAddress_;
    };
}