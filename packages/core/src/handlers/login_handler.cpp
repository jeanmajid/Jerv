#include <jerv/core/network_connection.hpp>
#include <jerv/common/logger.hpp>

namespace jerv::core {
    void NetworkConnection::handleLogin(binary::cursor &cursor) {
        protocol::LoginPacket packet;
        packet.deserialize(cursor);
        packet.parsePayload();

        if (packet.identity) {
            playerName_ = packet.identity->displayName;
            playerUuid_ = packet.identity->uuid;
            playerXuid_ = packet.identity->xuid;
        }

        JERV_LOG_INFO("Player {} connected (XUID: {})", playerName_, playerXuid_);

        protocol::playStatusPacket loginStatus;
        loginStatus.status = protocol::PlayStatus::LoginSuccess;
        send(loginStatus);

        protocol::ResourcePacksInfoPacket resourcesInfo;
        resourcesInfo.mustAccept = false;
        resourcesInfo.hasAddons = false;
        resourcesInfo.hasScripts = false;
        resourcesInfo.forceDisableVibrantVisuals = false;
        resourcesInfo.worldTemplateUuid = "00000000-0000-0000-0000-000000000000";
        resourcesInfo.worldTemplateVersion = "";
        resourcesInfo.packs = {};
        send(resourcesInfo);
    }
}