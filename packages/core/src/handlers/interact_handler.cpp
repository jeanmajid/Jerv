#include <jerv/protocol/packets/interact.hpp>
#include <jerv/common/logger.hpp>

#include "jerv/core/network_connection.hpp"

namespace jerv::core {
    void NetworkConnection::handleInteract(binary::cursor &cursor) {
        protocol::InteractPacket packet;
        packet.deserialize(cursor);

        // LOG_INFO("Action: {}, runtimeEntityId: {}, mousePos: ({}, {}, {})", packet.actionId, packet.targetEntityId, packet.position.x, packet.position.y, packet.position.z);
    }
}