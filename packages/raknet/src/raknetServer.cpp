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


#include <cstdint>
#include <jerv/raknet/raknetServer.hpp>
#include <jerv/common/logger.hpp>
#include <jerv/binary/cursor.hpp>

using jerv::common::Logger;

namespace jerv::raknet {
    RaknetServer::RaknetServer(asio::io_context &io, int16_t port) : ioContext(io),
                                                                     socket(ioContext, asio::ip::udp::endpoint(
                                                                                asio::ip::udp::v4(), port)) {
        receive();
    }

    void RaknetServer::receive() {
        socket.async_receive_from(asio::buffer(receiveBuffer), endpoint, [this](std::error_code errorCode, size_t bytesTransfered) {
            if (!errorCode && bytesTransfered > 0) {
                // todo: instead of doing allocation here, use a memory pool
                std::vector packetData(receiveBuffer.begin(), receiveBuffer.begin() + bytesTransfered);
                asio::ip::udp::endpoint sender = endpoint;

                receive();

                handlePacket(sender, packetData);
            } else {
                receive();
            }
        });
    }

    void RaknetServer::handlePacket(const asio::ip::udp::endpoint &sender, std::vector<uint8_t> data) {
        binary::Cursor cursor(data);


    }
}
