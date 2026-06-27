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
#include <jerv/protocol/enums.hpp>
#include <variant>

namespace jerv::protocol {
    enum class MetaDataDictionaryType : int32_t {
        Byte = 0,
        Short = 1,
        Int = 2,
        Float = 3,
        String = 4,
        Compound = 5,
        Vector3 = 6,
        Long = 7,
        Vector3f = 8
    };

    using ValueType = std::variant<
        int8_t,
        int16_t,
        int32_t,
        int64_t,
        float,
        std::string,
        BlockLocation,
        Vec3f
    >;

    struct MetaDataDictionary {
        int32_t key;
        MetaDataDictionaryType type;
        ValueType value;

        void serialize(binary::Cursor &cursor) const {
            cursor.writeVarInt32(key);
            cursor.writeVarInt32(static_cast<int32_t>(type));

            switch (type) {
                case MetaDataDictionaryType::Byte: {
                    cursor.writeUint8(std::get<int8_t>(value));
                    break;
                }
                case MetaDataDictionaryType::Short: {
                    cursor.writeInt16<true>(std::get<int16_t>(value));
                    break;
                }
                case MetaDataDictionaryType::Int: {
                    cursor.writeZigZag32(std::get<int32_t>(value));
                    break;
                }
                case MetaDataDictionaryType::Float: {
                    cursor.writeFloat32<true>(std::get<float>(value));
                    break;
                }
                case MetaDataDictionaryType::String: {
                    cursor.writeString(std::get<std::string>(value));
                    break;
                }
                case MetaDataDictionaryType::Compound: {
                    throw new std::runtime_error("Nbt not implemented");
                    break;
                }
                case MetaDataDictionaryType::Vector3: {
                    std::get<BlockLocation>(value).serialize(cursor);
                    break;
                }
                case MetaDataDictionaryType::Long: {
                    cursor.writeZigZag64(std::get<int64_t>(value));
                    break;
                }
                case MetaDataDictionaryType::Vector3f: {
                    std::get<Vec3f>(value).serialize(cursor);
                    break;
                }
            }
        }

        void deserialize(binary::Cursor &cursor) {
        }
    };

    struct Properties {
        std::vector<int32_t> ints;
        std::vector<float> floats;

        void serialize(binary::Cursor &cursor) const {
            cursor.writeVarInt32(ints.size());
            for (const int &i: ints) {
                cursor.writeZigZag32(i);
            }

            cursor.writeVarInt32(floats.size());
            for (const float &f: floats) {
                cursor.writeFloat32<true>(f);
            }
        }

        void deserialize(binary::Cursor &cursor) {
        }
    };

    class SetActorDataPacket : public PacketType {
    public:
        int64_t runtimeEntityId;
        std::vector<MetaDataDictionary> metaData;
        Properties properties;
        int64_t tick;

        static constexpr auto ID = PacketId::SetActorData;
        PacketId getPacketId() const override {
            return PacketId::SetActorData;
        }

        void serialize(binary::Cursor &cursor) const override {
            cursor.writeVarInt64(runtimeEntityId);

            cursor.writeVarInt32(metaData.size());
            for (const MetaDataDictionary &meta: metaData) {
                meta.serialize(cursor);
            }

            properties.serialize(cursor);

            cursor.writeVarInt64(tick);
        }

        void deserialize(binary::Cursor &cursor) override {
        }
    };
}
