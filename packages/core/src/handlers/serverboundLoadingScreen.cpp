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
#include "jerv/protocol/packets/playStatus.hpp"
#include "jerv/protocol/packets/serverBoundLoadingScreen.hpp"

namespace jerv::core::handler {
    void handleServerboundLoadingScreenPacket(Jerver &server, raknet::ServerConnection &connection,
                                              binary::Cursor &cursor) {
        // TODO: check if this actually can be send multiple time, I guess it can
        if (!connection.playerSpawned) {
            connection.playerSpawned = true;
            protocol::PlayStatusPacket spawnStatus;
            spawnStatus.status = protocol::PlayStatus::PlayerSpawn;
            server.send(connection, spawnStatus);
            JERV_LOG_INFO("spawned {}", connection.playerName);
        }
    }

    static PacketRegistrar<protocol::ServerBoundLoadingScreenPacket> regServerBoundLoadingScreen{&handleServerboundLoadingScreenPacket};
}
