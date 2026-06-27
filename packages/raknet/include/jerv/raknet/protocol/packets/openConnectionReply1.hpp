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
#include "jerv/raknet/protocol/raknetBasePacket.hpp"

namespace jerv::raknet {
    class OpenConnectionReply1Packet : public RaknetBasePacket {
    public:
        int64_t serverGuid;
        bool serverHasSecurity;
        uint32_t cookie;
        uint16_t mtuSize;

        RaknetPacketId getPacketId() const override {
            return RaknetPacketId::OpenConnectionReply1;
        }

        void serialize(binary::Cursor &cursor) const override {
            cursor.writeMagic();
            cursor.writeInt64(serverGuid);
            cursor.writeBool(serverHasSecurity);
            if (serverHasSecurity) {
                cursor.writeUint32(cookie);
            }

            cursor.writeUint16(mtuSize);
        }

        void deserialize(binary::Cursor &cursor) override {
            (void) cursor;
        }
    };
}
