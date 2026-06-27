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
namespace jerv::protocol {
    struct Vec3f {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;

        void serialize(binary::Cursor &cursor) const {
            cursor.writeFloat32<true>(x);
            cursor.writeFloat32<true>(y);
            cursor.writeFloat32<true>(z);
        }

        void deserialize(binary::Cursor &cursor) {
            x = cursor.readFloat32<true>();
            y = cursor.readFloat32<true>();
            z = cursor.readFloat32<true>();
        }
    };

    struct Vec2f {
        float x = 0.0f;
        float z = 0.0f;

        void serialize(binary::Cursor &cursor) const {
            cursor.writeFloat32<true>(x);
            cursor.writeFloat32<true>(z);
        }

        void deserialize(binary::Cursor &cursor) {
            x = cursor.readFloat32<true>();
            z = cursor.readFloat32<true>();
        }
    };

    struct ChunkCoords {
        int32_t x = 0;
        int32_t z = 0;

        void serialize(binary::Cursor &cursor) const {
            cursor.writeZigZag32(x);
            cursor.writeZigZag32(z);
        }

        void deserialize(binary::Cursor &cursor) {
            x = cursor.readZigZag32();
            z = cursor.readZigZag32();
        }
    };

    struct BlockLocation {
        int32_t x = 0;
        int32_t y = 0;
        int32_t z = 0;

        void serialize(binary::Cursor &cursor) const {
            cursor.writeZigZag32(x);
            cursor.writeVarInt32(y);
            cursor.writeZigZag32(z);
        }

        void deserialize(binary::Cursor &cursor) {
            x = cursor.readZigZag32();
            y = cursor.readVarInt32();
            z = cursor.readZigZag32();
        }
    };

    enum class DimensionId : int32_t {
        Overworld = 0,
        Nether = 1,
        End = 2
    };

    enum class PermissionLevel : int8_t {
        Visitor = 0,
        Member = 1,
        Operator = 2,
        Custom = 3
    };
}