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
        using HandlerFn = void (NetworkConnection::*)(binary::Cursor &);
        using HandlerTable = std::array<HandlerFn, 0x149>;

        static const HandlerTable &handlerTable();

        void handlePacket(std::span<uint8_t> data);

        void handleRequestNetworkSettings(binary::Cursor &cursor);
        void handleLogin(binary::Cursor &cursor);
        void handleResourcePackClientResponse(binary::Cursor &cursor);
        void handleClientCacheStatus(binary::Cursor &cursor);
        void handleSetLocalPlayerAsInitialized(binary::Cursor &cursor);
        void handleRequestChunkRadius(binary::Cursor &cursor);
        void handlePlayerAuthInput(binary::Cursor &cursor);
        void handlePacketViolationWarning(binary::Cursor &cursor);
        void handleServerboundLoadingScreen(binary::Cursor &cursor);
        void handleServerboundDiagnostics(binary::Cursor &cursor);
        void handleText(binary::Cursor &cursor);
        void handleInteract(binary::Cursor &cursor);
        void handleCommandRequest(binary::Cursor &cursor);
        void handleSubChunkRequest(binary::Cursor &cursor);

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
