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
#include <vector>
#include <string>

namespace jerv::protocol {
    class AvailableActorIdentifiersPacket : public PacketType {
    public:
        static constexpr auto ID = PacketId::AvailableActorIdentifiers;
        PacketId getPacketId() const override {
            return PacketId::AvailableActorIdentifiers;
        }

        void serialize(binary::Cursor &cursor) const override {
            cursor.writeUint8(0x0A);
            cursor.writeUint8(0x00);
            cursor.writeUint8(0x09);
            cursor.writeUint8(0x06);

            cursor.writeUint8('i');
            cursor.writeUint8('d');
            cursor.writeUint8('l');
            cursor.writeUint8('i');
            cursor.writeUint8('s');
            cursor.writeUint8('t');

            cursor.writeUint8(0x0A);
            cursor.writeZigZag32(0);

            cursor.writeUint8(0x00);
        }

        void deserialize(binary::Cursor &cursor) override {
        }
    };
}