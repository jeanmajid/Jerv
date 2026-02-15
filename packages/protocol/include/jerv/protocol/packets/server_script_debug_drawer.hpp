#pragma once

#include <jerv/protocol/packet.hpp>
#include <jerv/protocol/enums.hpp>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace jerv::protocol {
    class ServerScriptDebugDrawerPacket : public PacketType {
    public:
        enum ShapeType : uint8_t {
            Line = 0,
            Box = 1,
            Sphere = 2,
            Circle = 3,
            Text = 4,
            Arrow = 5
        };

        struct ShapeColor {
            uint8_t red = 0;
            uint8_t green = 0;
            uint8_t blue = 0;
            uint8_t alpha = 255;

            void serialize(binary::Cursor &cursor) const {
                const uint32_t argb = static_cast<uint32_t>(alpha) << 24 |
                                      static_cast<uint32_t>(red) << 16 |
                                      static_cast<uint32_t>(green) << 8 |
                                      static_cast<uint32_t>(blue);
                cursor.writeUint32<true>(argb);
            }

            void deserialize(binary::Cursor &cursor) {
                const uint32_t argb = cursor.readUint32<true>();
                alpha = static_cast<uint8_t>((argb >> 24) & 0xFF);
                red = static_cast<uint8_t>((argb >> 16) & 0xFF);
                green = static_cast<uint8_t>((argb >> 8) & 0xFF);
                blue = static_cast<uint8_t>(argb & 0xFF);
            }
        };

        struct Shape {
            int64_t runtimeId = 0;
            DimensionId dimension = DimensionId::Overworld;

            std::optional<ShapeType> type;
            std::optional<Vec3f> location;
            std::optional<float> scale;
            std::optional<Vec3f> rotation;
            std::optional<float> totalTimeLeft;
            std::optional<ShapeColor> color;
            std::optional<std::string> text;
            std::optional<Vec3f> boxBound;
            std::optional<Vec3f> lineEndLocation;
            std::optional<float> arrowHeadLength;
            std::optional<float> arrowHeadRadius;
            std::optional<uint8_t> numSegments;

            void serialize(binary::Cursor &cursor) const {
                cursor.writeZigZag64(runtimeId);

                cursor.writeBool(type.has_value());
                if (type) cursor.writeUint8(*type);

                cursor.writeBool(location.has_value());
                if (location) location->serialize(cursor);

                cursor.writeBool(scale.has_value());
                if (scale) cursor.writeFloat32<true>(*scale);

                cursor.writeBool(rotation.has_value());
                if (rotation) rotation->serialize(cursor);

                cursor.writeBool(totalTimeLeft.has_value());
                if (totalTimeLeft) cursor.writeFloat32<true>(*totalTimeLeft);

                cursor.writeBool(color.has_value());
                if (color) color->serialize(cursor);

                cursor.writeVarInt32(static_cast<int32_t>(dimension));
                int payloadType = 0;
                if (type.has_value()) {
                    switch (*type) {
                        case Arrow: payloadType = 1; break;
                        case Text: payloadType = 2; break;
                        case Box: payloadType = 3; break;
                        case Line: payloadType = 4; break;
                        case Sphere:
                        case Circle: payloadType = 5; break;
                        default: payloadType = 0; break;
                    }
                }
                cursor.writeVarInt32(payloadType);

                if (type.has_value()) {
                    switch (*type) {
                        case Arrow:
                            cursor.writeBool(lineEndLocation.has_value());
                            if (lineEndLocation) lineEndLocation->serialize(cursor);
                            cursor.writeBool(arrowHeadLength.has_value());
                            if (arrowHeadLength) cursor.writeFloat32<true>(*arrowHeadLength);
                            cursor.writeBool(arrowHeadRadius.has_value());
                            if (arrowHeadRadius) cursor.writeFloat32<true>(*arrowHeadRadius);
                            cursor.writeBool(numSegments.has_value());
                            if (numSegments) cursor.writeUint8(*numSegments);
                            break;
                        case Box:
                            (boxBound ? *boxBound : Vec3f{}).serialize(cursor);
                            break;
                        case Line:
                            (lineEndLocation ? *lineEndLocation : Vec3f{}).serialize(cursor);
                            break;
                        case Sphere: // or cirlce
                            cursor.writeUint8(numSegments.value_or(0));
                            break;
                        case Text:
                            cursor.writeString(text.value_or(""));
                            break;
                    }
                }
            }

            void deserialize(binary::Cursor &cursor) {}
        };

        std::vector<Shape> shapes;

        PacketId getPacketId() const override {
            return PacketId::ServerScriptDebugDrawer;
        }

        void serialize(binary::Cursor &cursor) const override {
            cursor.writeVarInt32(static_cast<int32_t>(shapes.size()));
            for (const auto &shape: shapes) {
                shape.serialize(cursor);
            }
        }

        void deserialize(binary::Cursor &cursor) override {
        }
    };
}
