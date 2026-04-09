#include "jerv/core/jerver.hpp"
#include "jerv/core/packetHandler.hpp"
#include "jerv/protocol/packets/requestNetworkSettings.hpp"

namespace jerv::core {
    void handleRequestNetworkSettingsPacket(Jerver &server, raknet::ServerConnection &connection,
                      binary::Cursor &cursor) {
        protocol::RequestNetworkSettingsPacket requestNetworkSettings;
        requestNetworkSettings.deserialize(cursor);

        protocol::NetworkSettingsPacket networkSettings;
        networkSettings.compressionAlgorithm = protocol::CompressionAlgorithm::NoCompression;
        networkSettings.compressionThreshold = 0;
        networkSettings.clientThrottleEnabled = false;

        server.send(connection, networkSettings);
        connection.networkSettingsSent = true;
    }

    static PacketRegistrar<protocol::RequestNetworkSettingsPacket> reg{&handleRequestNetworkSettingsPacket};
}
