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
#include <asio/ip/udp.hpp>

namespace jerv::raknet {
    class RaknetServer {
    public:
        asio::io_context& ioContext;
        asio::ip::udp::socket socket;
        asio::ip::udp::endpoint endpoint;

        RaknetServer(asio::io_context& io, int16_t port);
    private:
        void receive();
        void handlePacket(const asio::ip::udp::endpoint& sender, std::vector<uint8_t> data);

        std::array<uint8_t, 2300> receiveBuffer;
    };
}
