#pragma once

#include <jerv/raknet/interfaces.hpp>
#include <jerv/raknet/server_connection.hpp>
#include <jerv/raknet/constants.hpp>
#include <map>
#include <memory>
#include <functional>
#include <thread>
#include <atomic>

namespace jerv::raknet {
    class ServerConnectionListener : public ServerListener {
    public:
        static constexpr int STALE_CONNECTION_ELIMINATION_INTERVAL = 3000;
        static constexpr int STALE_CONNECTION_LIFETIME_MAX_AGE = 15000;

        ServerConnectionListener();

        virtual ~ServerConnectionListener();
        void addListenerSource(SocketSource &source) override;
        virtual std::vector<uint8_t> getMOTD(const AddressInfo &receiver);

        std::function<void(const std::string &)> onErrorHandler;
        std::function<void(ServerConnection *)> onNewConnection;
        std::function<void(ServerConnection *)> onConnectionDisconnected;

        const std::map<std::string, std::unique_ptr<ServerConnection> > &connections() const {
            return connections_;
        }

        uint64_t guid() const { return guid_; }

        void onMessage(SocketSource &source, std::span<uint8_t> msg, const AddressInfo &endpoint);

    protected:
        void handleUnconnectedPing(const SocketSource &source, std::span<uint8_t> data, const AddressInfo &receiver);
        void handleOpenConnectionRequestOne(const SocketSource &source, std::span<uint8_t> data, const AddressInfo &receiver) const;
        void handleOpenConnectionRequestTwo(SocketSource &source, std::span<uint8_t> data, const AddressInfo &receiver);

        std::map<std::string, std::unique_ptr<ServerConnection> > connections_;
        uint64_t guid_;

    private:
        void runStaleConnectionChecker();

        std::atomic<bool> running_;
        std::thread staleCheckerThread_;
    };
}