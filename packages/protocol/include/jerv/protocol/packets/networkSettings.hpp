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
    enum class CompressionAlgorithm : uint8_t {
        Zlib = 0,
        Snappy = 1,
        NoCompression = 0xFF
    };

    class NetworkSettingsPacket : public PacketType {
    public:
        uint16_t compressionThreshold = 1;
        CompressionAlgorithm compressionAlgorithm;
        bool clientThrottleEnabled = false;
        uint8_t clientThrottleThreshold = 0;
        float clientThrottleScalar = 0.0f;

        static constexpr auto ID = PacketId::NetworkSettings;
        PacketId getPacketId() const override {
            return PacketId::NetworkSettings;
        }

        void serialize(binary::Cursor &cursor) const override {
            cursor.writeUint16<true>(compressionThreshold);
            cursor.writeUint16<true>(static_cast<uint16_t>(compressionAlgorithm));
            cursor.writeUint8(clientThrottleEnabled ? 1 : 0);
            cursor.writeUint8(clientThrottleThreshold);
            cursor.writeFloat32<true>(clientThrottleScalar);
        }

        void deserialize(binary::Cursor &cursor) override {
            compressionThreshold = cursor.readUint16<true>();
            compressionAlgorithm = static_cast<CompressionAlgorithm>(cursor.readUint16<true>());
            clientThrottleEnabled = cursor.readUint8() != 0;
            clientThrottleThreshold = cursor.readUint8();
            clientThrottleScalar = cursor.readFloat32<true>();
        }
    };
}
