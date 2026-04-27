#include "jerv/core/jerver.hpp"
#include "jerv/core/packetHandler.hpp"
#include "jerv/protocol/packets/login.hpp"
#include "jerv/protocol/packets/playStatus.hpp"
#include "jerv/protocol/packets/resourcePacksInfo.hpp"

namespace jerv::core::handler {
    void handleLoginPacket(Jerver &server, raknet::ServerConnection &connection,
                      binary::Cursor &cursor) {
        protocol::LoginPacket loginPacket;
        loginPacket.deserialize(cursor);

        if (loginPacket.microsoftAuth) {
            connection.playerName = loginPacket.microsoftAuth->xboxGamertag;
            // TODO: Fix uuid, hash xuid to get it. identity got deprecated
            // playerUuid_ = packet.identity->uuid;
            connection.playerXuid = loginPacket.microsoftAuth->xuid;
        }

        JERV_LOG_INFO("Player {} connected (XUID: {})", connection.playerName, connection.playerXuid);

        protocol::PlayStatusPacket loginStatus;
        loginStatus.status = protocol::PlayStatus::LoginSuccess;
        server.send(connection, loginStatus);

        protocol::ResourcePacksInfoPacket resourcesInfo;
        resourcesInfo.mustAccept = false;
        resourcesInfo.hasAddons = false;
        resourcesInfo.hasScripts = false;
        resourcesInfo.forceDisableVibrantVisuals = false;
        resourcesInfo.worldTemplateUuid = "00000000-0000-0000-0000-000000000000";
        resourcesInfo.worldTemplateVersion = "";
        resourcesInfo.packs = {};
        server.send(connection, resourcesInfo);
    }

    static PacketRegistrar<protocol::LoginPacket> regLogin{&handleLoginPacket};
}
