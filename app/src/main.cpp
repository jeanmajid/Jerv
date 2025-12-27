#include <jerv/core/jerver.hpp>
#include <jerv/core/events.hpp>
#include <jerv/protocol/protocol.hpp>
#include <jerv/common/logger.hpp>
#include <iostream>

int main() {
    try {
        jerv::Jerver jerver;

        // jerver.onPacketType(jerv::protocol::PacketId::Animate, [](jerv::core::PacketEvent& event) {
        //     LOG_INFO("Swing");
        // });

        jerver.onPlayerJoin = [](jerv::core::ConnectionEvent &event) {
            JERV_LOG_INFO("player joined");
        };

        jerver.onPlayerLeave = [](jerv::core::ConnectionEvent &event) {
            JERV_LOG_INFO("player left");
        };

        jerver.server().bindV4();
        jerver.start();
    } catch (const std::exception &e) {
        JERV_LOG_ERROR("fatal error: {}", e.what());
    }

    return 0;
}
