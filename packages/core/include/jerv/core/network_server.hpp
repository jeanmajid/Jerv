#pragma once

#include <jerv/core/raknet_listener.hpp>
#include <jerv/binary/cursor.hpp>
#include <asio.hpp>
#include <map>
#include <memory>

#include "constants.hpp"

namespace jerv::core {
    class NetworkConnection;
}

namespace jerv::core {
    class NetworkServer {
    public:
        explicit NetworkServer(Jerver &jerver);

        ~NetworkServer();

        void bindV4(uint16_t port = NETWORK_LAN_DISCOVERY_PORT4, const std::string &address = NETWORK_ANY_ADDRESS4);
        void bindV6(uint16_t port = NETWORK_LAN_DISCOVERY_PORT6, const std::string &address = NETWORK_ANY_ADDRESS6);

        binary::ResizableCursor &singlePacketCursor() { return singlePacketCursor_; }
        binary::ResizableCursor &multiPacketCursor() { return multiPacketCursor_; }

        void poll();
        void tick();

        class ConnectionsView {
        public:
            using MapIter = std::map<raknet::ServerConnection *, std::unique_ptr<NetworkConnection> >::iterator;

            class Iterator {
            public:
                explicit Iterator(const MapIter it) : it_(it) {
                }

                NetworkConnection &operator*() const { return *it_->second; }
                NetworkConnection *operator->() const { return it_->second.get(); }

                Iterator &operator++() {
                    ++it_;
                    return *this;
                }

                bool operator==(const Iterator &other) const { return it_ == other.it_; }
                bool operator!=(const Iterator &other) const { return it_ != other.it_; }
            private:
                MapIter it_;
            };

            ConnectionsView(const MapIter begin, const MapIter end) : begin_(begin), end_(end) {
            }

            Iterator begin() const { return Iterator(begin_); }
            Iterator end() const { return Iterator(end_); }
        private:
            MapIter begin_;
            MapIter end_;
        };

        class ConstConnectionsView {
        public:
            using MapIter = std::map<raknet::ServerConnection *, std::unique_ptr<NetworkConnection> >::const_iterator;

            class Iterator {
            public:
                explicit Iterator(const MapIter it) : it_(it) {
                }

                const NetworkConnection &operator*() const { return *it_->second; }
                const NetworkConnection *operator->() const { return it_->second.get(); }

                Iterator &operator++() {
                    ++it_;
                    return *this;
                }

                bool operator==(const Iterator &other) const { return it_ == other.it_; }
                bool operator!=(const Iterator &other) const { return it_ != other.it_; }

            private:
                MapIter it_;
            };

            ConstConnectionsView(const MapIter begin, const MapIter end) : begin_(begin), end_(end) {
            }

            Iterator begin() const { return Iterator(begin_); }
            Iterator end() const { return Iterator(end_); }

        private:
            MapIter begin_;
            MapIter end_;
        };

        ConnectionsView connections() { return ConnectionsView(rawConnections_.begin(), rawConnections_.end()); }

        ConstConnectionsView connections() const {
            return ConstConnectionsView(rawConnections_.begin(), rawConnections_.end());
        }

        void onConnectionCreate(raknet::ServerConnection *connection);
        void onConnectionDiscard(raknet::ServerConnection *connection);
        void onConnectionReady(NetworkConnection &connection);

        Jerver &jerver() const { return jerver_; }

    private:
        Jerver &jerver_;
        RakNetListener raknet_;
        binary::ResizableCursor singlePacketCursor_;
        binary::ResizableCursor multiPacketCursor_;
        std::map<raknet::ServerConnection *, std::unique_ptr<NetworkConnection> > rawConnections_;

        asio::io_context ioContext_;
        std::unique_ptr<asio::ip::udp::socket> socket4_;
        std::unique_ptr<asio::ip::udp::socket> socket6_;
        raknet::SocketSource socketSource4_;
        raknet::SocketSource socketSource6_;

        std::vector<uint8_t> receiveBuffer_;
        asio::ip::udp::endpoint remoteEndpoint_;

        void startReceive(asio::ip::udp::socket &socket);

        void handleReceive(asio::ip::udp::socket &socket, const asio::error_code &error, size_t bytesReceived);
    };
}