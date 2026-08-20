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

namespace jerv::raknet {
    enum class RaknetPacketId {
        ConnectedPing = 0x00,
        ConnectedPong = 0x03,
        UnconnectPing = 0x01,
        UnconnectPong = 0x1c,
        OpenConnectionRequest1 = 0x05,
        OpenConnectionReply1 = 0x06,
        OpenConnectionRequest2 = 0x07,
        OpenConnectionReply2 = 0x08,
        ConnectionRequest = 0x09,
        ConnectionRequestAccepted = 0x10,
        NewIncomingConnection = 0x13,
        AckDatagram = 0xC0,
        NackDatagram = 0xA0,
        Disconnect = 0x15,
        GameData = 0xFE
    };
}