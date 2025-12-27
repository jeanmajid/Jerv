#include <jerv/raknet/server_connection_listener.hpp>
#include <jerv/raknet/proto.hpp>
#include <chrono>
#include <thread>
#include <iostream>

// TODO: Stale checker is currently dangerous, if the connections update while its running, it segfaults

namespace jerv::raknet {
    ServerConnectionListener::ServerConnectionListener()
        : guid_(random64())
          , running_(true) {
        staleCheckerThread_ = std::thread([this]() {
            runStaleConnectionChecker();
        });
    }

    ServerConnectionListener::~ServerConnectionListener() {
        running_ = false;
        if (staleCheckerThread_.joinable()) {
            staleCheckerThread_.join();
        }
    }

    void ServerConnectionListener::addListenerSource(SocketSource &source) {
        source.onDataCallback([this, &source](const std::span<uint8_t> msg, const AddressInfo &rinfo) {
            onMessage(source, msg, rinfo);
        });
    }

    std::vector<uint8_t> ServerConnectionListener::getMOTD(const AddressInfo &receiver) {
        std::string motd = "MCPE;Jerver;390;1.14.60;16;50;" + std::to_string(guid_) + ";The New World;";
        return std::vector<uint8_t>(motd.begin(), motd.end());
    }

    void ServerConnectionListener::onMessage(SocketSource &source, std::span<uint8_t> msg,
                                             const AddressInfo &endpoint) {
        const uint8_t packetId = msg[0];

        if (packetId & 0x80) {
            std::string id = BaseConnection::getIdentifierFor(endpoint);
            auto it = connections_.find(id);
            if (it != connections_.end()) {
                ServerConnection *conn = it->second.get();
                conn->handleIncoming(msg);
            }
            return;
        }

        switch (packetId) {
            case 0x01:
                handleUnconnectedPing(source, msg, endpoint);
                break;
            case 0x05:
                handleOpenConnectionRequestOne(source, msg, endpoint);
                break;
            case 0x07:
                handleOpenConnectionRequestTwo(source, msg, endpoint);
                break;
            default:
                if (onErrorHandler) {
                    onErrorHandler("unhandled packet: 0x" + std::to_string(packetId));
                }
                break;
        }
    }

    void ServerConnectionListener::handleUnconnectedPing(const SocketSource &source, const std::span<uint8_t> data,
                                                         const AddressInfo &receiver) {
        uint64_t pingTime = proto::getUnconnectedPingTime(data);
        std::vector<uint8_t> motd = getMOTD(receiver);
        std::vector<uint8_t> buffer = proto::rentUnconnectedPongBufferWith(pingTime, guid_, motd);
        source.send(buffer, receiver);
    }

    void ServerConnectionListener::handleOpenConnectionRequestOne(const SocketSource &source,
                                                                  const std::span<uint8_t> data,
                                                                  const AddressInfo &receiver) const {
        auto mtu = static_cast<uint16_t>(data.size() + UDP_HEADER_SIZE);
        if (mtu > IDEAL_MAX_MTU_SIZE) {
            mtu = IDEAL_MAX_MTU_SIZE;
        }

        std::vector<uint8_t> buffer = proto::rentOpenConnectionReplyOneBufferWith(guid_, mtu);
        source.send(buffer, receiver);
    }

    void ServerConnectionListener::handleOpenConnectionRequestTwo(SocketSource &source, const std::span<uint8_t> data,
                                                                  const AddressInfo &receiver) {
        auto info = proto::getOpenConnectionRequestTwoInfo(data);

        std::vector<uint8_t> buffer = proto::rentOpenConnectionReplyTwoBufferWith(guid_, receiver, info.mtu);
        source.send(buffer, receiver);

        std::string id = BaseConnection::getIdentifierFor(receiver);
        if (connections_.contains(id)) {
            return;
        }

        auto connection = std::make_unique<ServerConnection>(
            source, receiver, info.serverAddress, info.guid, info.mtu
        );

        ServerConnection *connPtr = connection.get();

        connection->onConnectionEstablishedHandle = [this, connPtr]() {
            if (onNewConnection) {
                onNewConnection(connPtr);
            }
        };

        connection->onErrorHandle = [this](const std::string &error) {
            if (onErrorHandler) {
                onErrorHandler(error);
            }
        };

        connection->onDisconnect = [this, id]() {
            const auto it = connections_.find(id);
            if (it != connections_.end()) {
                if (onConnectionDisconnected) {
                    onConnectionDisconnected(it->second.get());
                }
                connections_.erase(it);
            }
        };

        connections_[id] = std::move(connection);
    }

    void ServerConnectionListener::runStaleConnectionChecker() {
        while (running_) {
            // clion is complaining about this one for no reason, just ignore it, it works perfectly fine
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));

            auto now = std::chrono::steady_clock::now();
            std::vector<std::string> toRemove;

            for (auto &[id, connection]: connections_) {
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                    now - connection->incomingLastActivity
                ).count();

                if (elapsed > 4000) {
                    connection->disconnect();
                    if (connection->onDisconnect) {
                        connection->onDisconnect();
                    }
                    toRemove.push_back(id);
                }
            }

            for (const auto &id: toRemove) {
                auto it = connections_.find(id);
                if (it != connections_.end()) {
                    if (onConnectionDisconnected) {
                        onConnectionDisconnected(it->second.get());
                    }
                    connections_.erase(it);
                }
            }
        }
    }
}
