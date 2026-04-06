#pragma once
#include <asio.hpp>

#include "constants.hpp"
#include "frameCapsule.hpp"
#include "serverConnection.hpp"
#include "jerv/binary/cursor.hpp"
#include "protocol/packetIds.hpp"

namespace jerv::raknet {
    class RaknetBasePacket;

    class RaknetServer {
    public:
        void bindV4(uint16_t port = NETWORK_LAN_DISCOVERY_PORT4, const std::string &address = NETWORK_ANY_ADDRESS4);

        void bindV6();

        void start();

        template<size_t BufferSize = IDEAL_MAX_MTU_SIZE>
        void sendPacket(const asio::ip::udp::endpoint &endpoint, const RaknetBasePacket &packet);

    private:
        void startReceive(asio::ip::udp::socket &socket);

        void onMessage(const asio::ip::udp::endpoint &endpoint, std::span<uint8_t> data);

        void handleOnline(const asio::ip::udp::endpoint &endpoint, binary::Cursor &cursor);

        void handleOffline(const asio::ip::udp::endpoint &endpoint, uint8_t packetId, binary::Cursor &cursor);

        void handleFrameSet(ServerConnection &connection, binary::Cursor &cursor);

        std::vector<std::pair<uint32_t, uint32_t> > getRangesFromSequence(const std::vector<uint32_t> &sequence);


        FrameCapsule handleCapsule(ServerConnection &connection, binary::Cursor &cursor);

        void handleFragment(ServerConnection &connection, const FrameCapsule &capsule);

        void handleAck(const ServerConnection &connection, binary::Cursor &cursor);

        void handleNack(const ServerConnection &connection, binary::Cursor &cursor);

        std::string endpointToString(const asio::ip::udp::endpoint &endpoint);

        void sendAck(ServerConnection &connection, RaknetPacketId type,
                     const std::vector<std::pair<uint32_t, uint32_t> > &ranges);

        void handleFrame(const ServerConnection &connection, const std::span<uint8_t> &span);

        int64_t serverGuid = 0;

        asio::io_context _ioContext;
        std::unique_ptr<asio::ip::udp::socket> socket4;
        std::unique_ptr<asio::ip::udp::socket> socket6;

        std::array<uint8_t, 1500> receiveBuffer = {};
        asio::ip::udp::endpoint remoteEndpoint;

        std::unordered_map<std::string, ServerConnection> connections;
    };
}
