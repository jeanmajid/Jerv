#pragma once

#include <jerv/core/tick_manager.hpp>
#include <jerv/core/network_server.hpp>
#include <jerv/core/events.hpp>
#include <jerv/core/world/world.hpp>
#include <jerv/protocol/protocol.hpp>
#include <string>
#include <memory>
#include <functional>
#include <unordered_map>

namespace jerv {
    class Jerver {
    public:
        Jerver();
        ~Jerver();

        void start();
        void stop();

        core::TickManager &tick() { return tick_; }
        core::NetworkServer &server() const { return *server_; }
        core::World &world() { return world_; }

        const std::string providerName_ = "Jerver";

        core::PacketHandler onPacket;
        void onPacketType(protocol::PacketId id, core::PacketHandler handler);
        core::ConnectionHandler onPlayerJoin;
        core::ConnectionHandler onPlayerLeave;

        bool dispatchPacket(core::PacketEvent &event);
        void dispatchPlayerJoin(core::ConnectionEvent &event);
        void dispatchPlayerLeave(core::ConnectionEvent &event);
    private:
        core::TickManager tick_;
        std::unique_ptr<core::NetworkServer> server_;
        core::World world_;

        std::unordered_map<protocol::PacketId, core::PacketHandler> packetHandlers_;
    };
}