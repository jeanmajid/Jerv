#include <jerv/core/network_connection.hpp>
#include <jerv/common/logger.hpp>

namespace jerv::core {
    void NetworkConnection::handlePacketViolationWarning(binary::Cursor &cursor) {
        JERV_LOG_WARN("PacketViolationWarning received from client");
    }
}