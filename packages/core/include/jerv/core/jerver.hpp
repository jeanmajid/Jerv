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

#pragma once
#include "jerv/raknet/raknetServer.hpp"
#include "tick/tickManager.hpp"
#include "world/dimension.hpp"

namespace jerv::core {
    class Jerver {
    public:
        Jerver();

        void bindV4(uint16_t port = raknet::NETWORK_LAN_DISCOVERY_PORT4);

        void bindV6(uint16_t port = raknet::NETWORK_LAN_DISCOVERY_PORT6);

        void start();

        void send(raknet::ServerConnection &connection, const protocol::PacketType &packet);

    private:
        static void handleDataStatic(void *ctx, raknet::ServerConnection &connection, std::span<uint8_t> data);

        void handleData(raknet::ServerConnection &connection, std::span<uint8_t> data);

        void handlePacket(raknet::ServerConnection &connection, std::span<uint8_t> data);

        static void handleTickStatic(void *ctx, uint64_t tick);

        void handleTick(uint64_t tick);

        raknet::RaknetServer raknetServer;
        tick::TickManager tickManager;
        world::Dimension dimension{"overworld"};
    };
}
