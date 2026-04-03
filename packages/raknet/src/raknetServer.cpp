#include <iostream>
#include <jerv/raknet/raknetServer.hpp>

#include "jerv/binary/nbt.hpp"
#include "jerv/common/logger.hpp"
#include "jerv/raknet/constants.hpp"
#include "jerv/raknet/protocol/packetIds.hpp"
#include "jerv/raknet/protocol/packets/unconnectPing.hpp"
#include "jerv/raknet/protocol/packets/unconnectPong.hpp"

namespace jerv::raknet {
    void RaknetServer::bindV4(uint16_t port, const std::string &address) {
        _socket4 = std::make_unique<asio::ip::udp::socket>(ioContext_);
        _socket4->open(asio::ip::udp::v4());
        _socket4->bind(asio::ip::udp::endpoint(asio::ip::make_address(address), port));

        JERV_LOG_INFO("listening on {}:{}", address, port);

        startReceive(*_socket4);
    }

    void RaknetServer::bindV6() {
    }

    void RaknetServer::start() {
        ioContext_.run();
    }

    void RaknetServer::startReceive(asio::ip::udp::socket &socket) {
        socket.async_receive_from(
            asio::buffer(_receiveBuffer),
            remoteEndpoint,
            [&socket, this](const asio::error_code &error, const size_t bytesReceived) {
                if (!error && bytesReceived > 0) {
                    const std::span data(_receiveBuffer.data(), bytesReceived);
                    onMessage(remoteEndpoint, data);
                }

                startReceive(socket);
            });
    }

    void RaknetServer::onMessage(asio::ip::udp::endpoint &endpoint, const std::span<uint8_t> data) {
        binary::Cursor cursor(data);
        const RaknetPacketId packetId = static_cast<RaknetPacketId>(cursor.readUint8());

        switch (packetId) {
            case RaknetPacketId::UnconnectPing: {
                UnconnectPingPacket ping;
                ping.deserialize(cursor);

                UnconnectPongPacket pong;
                pong.clientAliveTimeMs = ping.clientAliveTimeMs;
                pong.serverGuid = 1231231;
                pong.motd = "MCPE;JERVER;975;1.0.0;100;200;1231231;JERVER;Survival;1;";
                std::vector<uint8_t> pongData(512);
                binary::Cursor pongCursor(pongData);
                pongCursor.writeUint8(0x1c);
                pong.serialize(pongCursor);

                _socket4->send_to(asio::buffer(pongData, pongCursor.processedBytesSize()),
                                  endpoint);
                break;
            }
            default: {
                JERV_LOG_DEBUG("Unhandled raknet packet id: {}", static_cast<uint8_t>(packetId));
                break;
            };
        }
    }
}
