#pragma once
#include <asio.hpp>

#include "constants.hpp"
#include "frameCapsule.hpp"
#include "reliability.hpp"
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
        void sendPacketRaw(const asio::ip::udp::endpoint &endpoint, const RaknetBasePacket &packet);

        void sendData(const asio::ip::udp::endpoint &endpoint, const std::span<uint8_t> buffer);

    private:
        void startReceive(asio::ip::udp::socket &socket);

        void onMessage(const asio::ip::udp::endpoint &endpoint, std::span<uint8_t> data);

        void handleOnline(const asio::ip::udp::endpoint &endpoint, binary::Cursor &cursor);

        void handleOffline(const asio::ip::udp::endpoint &endpoint, uint8_t packetId, binary::Cursor &cursor);

        void handleFrameSet(ServerConnection &connection, binary::Cursor &cursor);

        std::vector<std::pair<uint32_t, uint32_t> > getRangesFromSequence(const std::vector<uint32_t> &sequence);


        void handleCapsule(ServerConnection &connection, binary::Cursor &cursor);

        void handleFragment(ServerConnection &connection, const FrameCapsule &capsule);

        void handleAck(ServerConnection &connection, binary::Cursor &cursor);

        void handleNack(ServerConnection &connection, binary::Cursor &cursor);

        std::string endpointToString(const asio::ip::udp::endpoint &endpoint);


        void handleFrame(ServerConnection &connection, const std::span<uint8_t> &span);

        void sendAck(ServerConnection &connection, RaknetPacketId type,
                     const std::vector<std::pair<uint32_t, uint32_t> > &ranges);

        void sendFrame(ServerConnection &connection, std::span<uint8_t> data,
                       Reliability reliability);

        void sendCapsule(ServerConnection &connection, const FrameCapsule &frameCapsule, Reliability reliability);

        void processQueue(ServerConnection &connection);

        void createCurrentConnectionBuffer(ServerConnection &connection);

        int64_t serverGuid = 0;

        asio::io_context _ioContext;
        std::unique_ptr<asio::ip::udp::socket> socket4;
        std::unique_ptr<asio::ip::udp::socket> socket6;

        std::array<uint8_t, 2000> receiveBuffer = {};
        asio::ip::udp::endpoint remoteEndpoint;

        std::unordered_map<std::string, ServerConnection> connections;
    };
}
