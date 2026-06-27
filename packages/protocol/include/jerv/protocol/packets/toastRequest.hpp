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
#include <string>

namespace jerv::protocol {
    class ToastRequestPacket : public PacketType {
    public:
        std::string title;
        std::string content;

        static constexpr auto ID = PacketId::ToastRequest;
        PacketId getPacketId() const override {
            return PacketId::ToastRequest;
        }

        void serialize(binary::Cursor &cursor) const override {
            cursor.writeString(title);
            cursor.writeString(content);
        }

        void deserialize(binary::Cursor &cursor) override {
            title = cursor.readString();
            content = cursor.readString();
        }
    };
}


