#include <jerv/core/jerver.hpp>
#include <jerv/core/world/generator/overworld_generator.hpp>
#include <jerv/common/logger.hpp>
#include <iostream>

namespace jerv {
    Jerver::Jerver()
        : server_(std::make_unique<core::NetworkServer>(*this))
          , world_("overworld", 12345) {
        tick_.pollNetwork = [this]() {
            server_->poll();
            server_->tick();
        };

        core::TerrainGeneratorProperties props;
        props.seed = 12345;
        props.baseHeight = 64;
        props.heightVariation = 40;
        props.seaLevel = 63;
        world_.setGenerator<core::OverworldGenerator>(props);

        JERV_LOG_INFO("{} ready", providerName_);
        JERV_LOG_INFO("world: '{}', seed: {}", world_.name(), world_.seed());
    }

    Jerver::~Jerver() {
        stop();
    }

    void Jerver::start() {
        JERV_LOG_INFO("starting...");
        tick_.start();
    }

    void Jerver::stop() {
        tick_.stop();
    }

    void Jerver::onPacketType(protocol::PacketId id, core::PacketHandler handler) {
        packetHandlers_[id] = std::move(handler);
    }

    bool Jerver::dispatchPacket(core::PacketEvent &event) {
        if (onPacket) {
            onPacket(event);
            if (event.cancelled) {
                return false;
            }
        }

        auto it = packetHandlers_.find(event.packetId);
        if (it != packetHandlers_.end() && it->second) {
            it->second(event);
            if (event.cancelled) {
                return false;
            }
        }
        return true;
    }

    void Jerver::dispatchPlayerJoin(core::ConnectionEvent &event) {
        if (onPlayerJoin) {
            onPlayerJoin(event);
        }
    }

    void Jerver::dispatchPlayerLeave(core::ConnectionEvent &event) {
        if (onPlayerLeave) {
            onPlayerLeave(event);
        }
    }
}