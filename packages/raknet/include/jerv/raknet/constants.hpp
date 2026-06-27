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
#include <cstdint>

namespace jerv::raknet {
    constexpr uint16_t NETWORK_LAN_DISCOVERY_PORT4 = 19132;
    constexpr uint16_t NETWORK_LAN_DISCOVERY_PORT6 = 19133;
    constexpr const char *NETWORK_ANY_ADDRESS4 = "0.0.0.0";
    constexpr const char *NETWORK_LOOPBACK_ADDRESS4 = "127.0.0.1";
    constexpr const char *NETWORK_ANY_ADDRESS6 = "::0";
    constexpr const char *NETWORK_LOOPBACK_ADDRESS6 = "::1";

    constexpr uint8_t ONLINE_DATAGRAM_BIT_MASK = 0b1110'0000;
    constexpr uint8_t VALID_DATAGRAM_BIT = 0b1000'0000;
    constexpr uint8_t ACK_DATAGRAM_BIT = 0b0100'0000;
    constexpr uint8_t NACK_DATAGRAM_BIT = 0b0010'0000;
    constexpr uint8_t RELIABILITY_BIT_MASK = 0b1110'0000;
    constexpr uint8_t IS_FRAGMENTED_BIT = 0b0001'0000;

    constexpr size_t IDEAL_MAX_MTU_SIZE = 1432;
    constexpr size_t UDP_HEADER_SIZE = 68;
    constexpr size_t CAPSULE_FRAGMENT_META_SIZE = 10;
    constexpr size_t MAX_CAPSULE_HEADER_SIZE = CAPSULE_FRAGMENT_META_SIZE + 13;
    constexpr size_t MAX_FRAME_SET_HEADER_SIZE = MAX_CAPSULE_HEADER_SIZE + 4;

    constexpr bool IS_RELIABLE_LOOKUP[] = {false, false, true, true, true};
    constexpr bool IS_SEQUENCED_LOOKUP[] = {false, true, false, false, true};
    constexpr bool IS_ORDERED_LOOKUP[] = {false, true, false, true, true};
    constexpr bool IS_ORDERED_EXCLUSIVE_LOOKUP[] = {false, false, false, true, false, false, false, true};
}
