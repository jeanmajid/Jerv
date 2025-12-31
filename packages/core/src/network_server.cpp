#include <jerv/core/network_server.hpp>
#include <jerv/core/jerver.hpp>
#include <jerv/common/logger.hpp>
#include "jerv/core/network_connection.hpp"

namespace jerv::core {
    NetworkServer::NetworkServer(Jerver &jerver)
        : jerver_(jerver)
          , raknet_(jerver)
          , singlePacketCursor_(1024, 1 << 18)
          , multiPacketCursor_(1024, 1 << 20)
          , receiveBuffer_(2048) {
        raknet_.onNewConnection = [this](raknet::ServerConnection *conn) {
            onConnectionCreate(conn);
        };

        raknet_.onErrorHandler = [](const std::string &error) {
            JERV_LOG_ERROR("[raknet] {}", error);
        };

        raknet_.onConnectionDisconnected = [this](raknet::ServerConnection *conn) {
            onConnectionDiscard(conn);
        };
    }

    NetworkServer::~NetworkServer() {
    }

    void NetworkServer::bindV4(uint16_t port, const std::string &address) {
        socket4_ = std::make_unique<asio::ip::udp::socket>(ioContext_);
        socket4_->open(asio::ip::udp::v4());
        socket4_->bind(asio::ip::udp::endpoint(asio::ip::make_address(address), port));

        socketSource4_.send = [this](const std::span<const uint8_t> data, const raknet::AddressInfo &endpoint) {
            const asio::ip::udp::endpoint asioEndpoint(
                asio::ip::make_address(endpoint.address),
                endpoint.port
            );
            socket4_->send_to(asio::buffer(data.data(), data.size()), asioEndpoint);
        };

        startReceive(*socket4_);

        JERV_LOG_INFO("listening on {}:{}", address, port);
    }

    void NetworkServer::bindV6(uint16_t port, const std::string &address) {
        socket6_ = std::make_unique<asio::ip::udp::socket>(ioContext_);
        socket6_->open(asio::ip::udp::v6());
        socket6_->bind(asio::ip::udp::endpoint(asio::ip::make_address(address), port));

        socketSource6_.send = [this](const std::span<const uint8_t> data, const raknet::AddressInfo &endpoint) {
            const asio::ip::udp::endpoint asioEndpoint(
                asio::ip::make_address(endpoint.address),
                endpoint.port
            );
            socket6_->send_to(asio::buffer(data.data(), data.size()), asioEndpoint);
        };

        startReceive(*socket6_);
        JERV_LOG_INFO("listening on {}:{}", address, port);
    }

    void NetworkServer::startReceive(asio::ip::udp::socket &socket) {
        socket.async_receive_from(
            asio::buffer(receiveBuffer_),
            remoteEndpoint_,
            [this, &socket](const asio::error_code &error, const size_t bytesReceived) {
                handleReceive(socket, error, bytesReceived);
            }
        );
    }

    void NetworkServer::handleReceive(asio::ip::udp::socket &socket, const asio::error_code &error,
                                      const size_t bytesReceived) {
        if (!error && bytesReceived > 0) {
            raknet::AddressInfo endpoint;
            endpoint.address = remoteEndpoint_.address().to_string();
            endpoint.port = remoteEndpoint_.port();
            endpoint.family = remoteEndpoint_.address().is_v4() ? "IPv4" : "IPv6";

            raknet::SocketSource *source = nullptr;
            if (&socket == socket4_.get() && socketSource4_.send) {
                source = &socketSource4_;
            } else if (&socket == socket6_.get() && socketSource6_.send) {
                source = &socketSource6_;
            }

            if (source) {
                const std::span data(receiveBuffer_.data(), bytesReceived);
                raknet_.onMessage(*source, data, endpoint);
            }
        }

        startReceive(socket);
    }

    void NetworkServer::poll() {
        ioContext_.poll();
    }

    void NetworkServer::tick() {
        //TODO: if this array changes while running it blows up
        for (const auto &netConn: rawConnections_ | std::views::values) {
            netConn->tick(jerver_.world());
        }
    }

    void NetworkServer::onConnectionCreate(raknet::ServerConnection *connection) {
        auto netConn = std::make_unique<NetworkConnection>(*this, *connection);
        rawConnections_[connection] = std::move(netConn);
    }

    void NetworkServer::onConnectionDiscard(raknet::ServerConnection *connection) {
        rawConnections_.erase(connection);
    }

    void NetworkServer::onConnectionReady(NetworkConnection &connection) {
    }
}
