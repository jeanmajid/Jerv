#include <jerv/core/network_connection.hpp>
#include <jerv/common/logger.hpp>
#include <jerv/protocol/packets/request_network_settings.hpp>

namespace jerv::core {
    void NetworkConnection::handleRequestNetworkSettings(binary::Cursor &cursor) {
        protocol::RequestNetworkSettingsPacket packet;
        packet.deserialize(cursor);

        if (packet.clientNetworkVersion != protocol::PROTOCOL_VERSION) {
            JERV_LOG_WARN("protocol mismatch: {} != {}", packet.clientNetworkVersion,
                          protocol::PROTOCOL_VERSION);
        }

        send(networkSettings_);
        networkSettingsSet_ = true;
    }
}