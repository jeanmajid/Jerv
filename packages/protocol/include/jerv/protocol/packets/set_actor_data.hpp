#pragma once

#include <jerv/protocol/packet.hpp>
#include <jerv/protocol/enums.hpp>
#include <variant>

namespace jerv::protocol {
    class SetActorDataPacket : public PacketType {
    public:
        enum MetaDataDictionaryType : int32_t {
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
            int32_t type;
            ValueType value;

            void serialize(binary::cursor &cursor) const {
                cursor.writeVarInt32(key);
                cursor.writeVarInt32(type);

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

            void deserialize(binary::cursor &cursor) {
            }
        };

        struct Properties {
            std::vector<int32_t> ints;
            std::vector<float> floats;

            void serialize(binary::cursor &cursor) const {
                cursor.writeVarInt32(ints.size());
                for (const int &i: ints) {
                    cursor.writeZigZag32(i);
                }

                cursor.writeVarInt32(floats.size());
                for (const int &f: floats) {
                    cursor.writeFloat32<true>(f);
                }
            }

            void deserialize(binary::cursor &cursor) {
            }
        };

        int64_t runtimeEntityId;
        std::vector<MetaDataDictionary> metaData;
        Properties properties;
        int64_t tick;

        PacketId getPacketId() const override {
            return PacketId::SetActorData;
        }

        void serialize(binary::cursor &cursor) const override {
            cursor.writeVarInt64(runtimeEntityId);

            cursor.writeVarInt32(metaData.size());
            for (const MetaDataDictionary &meta: metaData) {
                meta.serialize(cursor);
            }

            properties.serialize(cursor);

            cursor.writeVarInt64(tick);
        }

        void deserialize(binary::cursor &cursor) override {
        }
    };
}
