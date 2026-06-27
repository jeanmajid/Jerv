/* SPDX-License-Identifier: LGPL-3.0-or-later
 * ============================================================================
 *  Jerv - Minecraft Bedrock Server Software
 *  Copyright (C) 2025-2026 jeanmajid
 *  https://github.com/jeanmajid/Jerv
 * ============================================================================
 *
 * This file is part of Jerv.
 *
 * Jerv is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Jerv is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Jerv. If not, see <https://www.gnu.org/licenses/>.
 */

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
