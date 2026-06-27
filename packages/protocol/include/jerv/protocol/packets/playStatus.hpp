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

#include <jerv/protocol/packet.hpp>

namespace jerv::protocol {
    enum class PlayStatus : int32_t {
        LoginSuccess = 0,
        FailedClient = 1,
        FailedServer = 2,
        PlayerSpawn = 3,
        FailedInvalidTenant = 4,
        FailedVanillaEdu = 5,
        FailedIncompatible = 6,
        FailedServerFull = 7,
        FailedEditorVanillaMismatch = 8,
        FailedVanillaEditorMismatch = 9
    };

    class PlayStatusPacket : public PacketType {
    public:
        PlayStatus status;

        static constexpr auto ID = PacketId::PlayStatus;
        PacketId getPacketId() const override {
            return PacketId::PlayStatus;
        }

        void serialize(binary::Cursor &cursor) const override {
            cursor.writeInt32(static_cast<int32_t>(status));
        }

        void deserialize(binary::Cursor &cursor) override {
            status = static_cast<PlayStatus>(cursor.readInt32());
        }
    };
}
