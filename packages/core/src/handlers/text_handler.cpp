#include <jerv/core/network_connection.hpp>
#include <jerv/common/logger.hpp>

#include "jerv/core/network_server.hpp"

namespace jerv::core {
    void NetworkConnection::handleText(binary::cursor &cursor) {
        protocol::TextPacket packet;
        packet.deserialize(cursor);

        for (auto &conn: server_.connections()) {
            conn.send(packet);
        }
    }
}