#include <jerv/core/network_connection.hpp>
#include <jerv/common/logger.hpp>
#include <jerv/protocol/packets/text_packet.hpp>

#include "jerv/core/network_server.hpp"

namespace jerv::core {
    void NetworkConnection::handleText(binary::Cursor &cursor) {
        protocol::TextPacket packet;
        packet.deserialize(cursor);
        JERV_LOG_INFO("test");
        JERV_LOG_INFO("tedasdst");
        JERV_LOG_INFO("tesasdt");
        JERV_LOG_INFO("tesadsast");
        JERV_LOG_INFO("testasd");
        JERV_LOG_INFO("test");

        for (auto &conn: server_.connections()) {
            conn.send(packet);
        }
    }
}