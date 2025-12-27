#pragma once
#include <cstdint>
namespace jerv::protocol {
    struct Vec3f {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;

        void serialize(binary::cursor &cursor) const {
            cursor.writeFloat32(x, true);
            cursor.writeFloat32(y, true);
            cursor.writeFloat32(z, true);
        }

        void deserialize(binary::cursor &cursor) {
            x = cursor.readFloat32(true);
            y = cursor.readFloat32(true);
            z = cursor.readFloat32(true);
        }
    };

    struct Vec2f {
        float x = 0.0f;
        float z = 0.0f;

        void serialize(binary::cursor &cursor) const {
            cursor.writeFloat32(x, true);
            cursor.writeFloat32(z, true);
        }

        void deserialize(binary::cursor &cursor) {
            x = cursor.readFloat32(true);
            z = cursor.readFloat32(true);
        }
    };

    struct ChunkCoords {
        int32_t x = 0;
        int32_t z = 0;

        void serialize(binary::cursor &cursor) const {
            cursor.writeZigZag32(x);
            cursor.writeZigZag32(z);
        }

        void deserialize(binary::cursor &cursor) {
            x = cursor.readZigZag32();
            z = cursor.readZigZag32();
        }
    };

    struct BlockLocation {
        int32_t x = 0;
        int32_t y = 0;
        int32_t z = 0;

        void serialize(binary::cursor &cursor) const {
            cursor.writeZigZag32(x);
            cursor.writeVarInt(y);
            cursor.writeZigZag32(z);
        }

        void deserialize(binary::cursor &cursor) {
            x = cursor.readZigZag32();
            y = cursor.readVarInt();
            z = cursor.readZigZag32();
        }
    };

    enum class DimensionId : int32_t {
        Overworld = 0,
        Nether = 1,
        End = 2
    };
}