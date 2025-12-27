#pragma once

#include <jerv/raknet/server_connection.hpp>
#include <jerv/protocol/protocol.hpp>
#include <jerv/binary/cursor.hpp>
#include <jerv/core/world/world.hpp>
#include <jerv/core/world/chunk_rendering.hpp>
#include <jerv/core/network_server.hpp>
#include <vector>
#include <memory>
#include <string>
#include <initializer_list>
#include <array>

#include "jerver.hpp"

namespace jerv::core {
    class NetworkConnection {
    public:
        NetworkConnection(NetworkServer &server, raknet::ServerConnection &connection);

        void handlePayload(std::span<uint8_t> message);

        void send(const std::vector<std::unique_ptr<protocol::PacketType> > &packets) const;
        void send(protocol::PacketType &packet) const;

        template<typename... Packets>
        void sendImmediate(Packets &... packets) {
            sendMultiple(packets...);
        }

        raknet::ServerConnection &connection() const { return connection_; }

        Jerver &jerver();

        void disconnect();

        const std::string &playerName() const { return playerName_; }
        const std::string &playerUuid() const { return playerUuid_; }
        const std::string &playerXuid() const { return playerXuid_; }


    private:
        using HandlerFn = void (NetworkConnection::*)(binary::cursor &);
        using HandlerTable = std::array<HandlerFn, 0x149>;

        static const HandlerTable &handlerTable();

        void handlePacket(std::span<uint8_t> data);

        void handleRequestNetworkSettings(binary::cursor &cursor);
        void handleLogin(binary::cursor &cursor);
        void handleResourcePackClientResponse(binary::cursor &cursor);
        void handleClientCacheStatus(binary::cursor &cursor);
        void handleSetLocalPlayerAsInitialized(binary::cursor &cursor);
        void handleRequestChunkRadius(binary::cursor &cursor);
        void handlePlayerAuthInput(binary::cursor &cursor);
        void handlePacketViolationWarning(binary::cursor &cursor);
        void handleServerboundLoadingScreen(binary::cursor &cursor);
        void handleServerboundDiagnostics(binary::cursor &cursor);
        void handleText(binary::cursor &cursor);
        void handleInteract(binary::cursor &cursor);
        void handleCommandRequest(binary::cursor &cursor);
        void handleSubChunkRequest(binary::cursor &cursor);

        void sendPacketSpan(std::span<protocol::PacketType * const> packets) const;
        void sendRaknetGamePacket(std::span<const uint8_t> message) const;

        void sendMultiple() {
        }

        template<typename First, typename... Rest>
        void sendMultiple(First &first, Rest &... rest) {
            sendMultipleImpl({&first, &rest...});
        }

        void sendMultipleImpl(std::initializer_list<protocol::PacketType *> packets) const;
        void sendBatch(const std::vector<protocol::PacketType *> &packets) const;

        NetworkServer &server_;
        raknet::ServerConnection &connection_;
        bool networkSettingsSet_ = false;
        protocol::NetworkSettingsPacket networkSettings_;

        std::string playerName_;
        std::string playerUuid_;
        std::string playerXuid_;

        float playerX_ = 0.0f;
        float playerY_ = 64.0f;
        float playerZ_ = 0.0f;

        ChunkRenderingManager chunkManager_;
        static constexpr int32_t MAX_VIEW_RADIUS = 100;
        static constexpr size_t MAX_DECOMPRESSED_SIZE = 16 * 1024 * 1024;
        bool playerSpawned_ = false;

    public:
        void tick(World &world);
        void updateChunks(World &world);
        const ChunkRenderingManager &chunkManager() const { return chunkManager_; }
    };
}
