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
#include <array>
#include <cstdint>

namespace jerv::binary {
    class Cursor;
}

namespace jerv::raknet {
    class ServerConnection;
}

namespace jerv::core {
    class Jerver;

    constexpr size_t MAX_PACKET_ID = 345;
    using HandlerFn = void(*)(Jerver&, raknet::ServerConnection&, binary::Cursor&);

    inline std::array<HandlerFn, MAX_PACKET_ID>& getHandlers() {
        static std::array<HandlerFn, MAX_PACKET_ID> handlers{};
        return handlers;
    }

    template<typename PacketType>
    struct PacketRegistrar {
        explicit PacketRegistrar(const HandlerFn handler) {
            const size_t packetId = static_cast<size_t>(PacketType::ID);
            static_assert(packetId > 0 && packetId <= MAX_PACKET_ID, "PacketId invalid");
            getHandlers()[packetId] = handler;
        }
    };
}

