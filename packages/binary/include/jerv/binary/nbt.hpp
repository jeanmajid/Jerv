#pragma once
#include <unordered_map>
#include <variant>
#include <string>

#include "cursor.hpp"
// Adjust include path if needed
// #include "jerv/common/logger.hpp"

namespace jerv::binary {
    enum NBTDataType : uint8_t {
        EndOfCompound = 0x00,
        Int8 = 0x01,
        Int16 = 0x02,
        Int32 = 0x03,
        Int64 = 0x04,
        Float = 0x05,
        Double = 0x06,
        Int8List = 0x07,
        String = 0x08,
        List = 0x09,
        Compound = 0x0A,
        Int32List = 0x0B,
        Int64List = 0x0C
    };

    struct NBTData;

    using NBTValueType = std::variant<
        int8_t,
        int16_t,
        int32_t,
        int64_t,
        float,
        double,
        std::string,
        std::vector<NBTData>,
        std::unordered_map<std::string, NBTData>,
        std::vector<int8_t>,
        std::vector<int32_t>,
        std::vector<int64_t> >;

    struct NBTData {
        NBTDataType type;
        NBTValueType value;

        void print(int indent = 0) const {
            std::string pad(indent * 2, ' ');

            switch (type) {
                case EndOfCompound:
                    break;
                case Int8:
                    JERV_LOG_INFO("{}(Int8) {}", pad, static_cast<int>(std::get<int8_t>(value)));
                    break;
                case Int16:
                    JERV_LOG_INFO("{}(Int16) {}", pad, std::get<int16_t>(value));
                    break;
                case Int32:
                    JERV_LOG_INFO("{}(Int32) {}", pad, std::get<int32_t>(value));
                    break;
                case Int64:
                    JERV_LOG_INFO("{}(Int64) {}", pad, std::get<int64_t>(value));
                    break;
                case Float:
                    JERV_LOG_INFO("{}(Float) {}", pad, std::get<float>(value));
                    break;
                case Double:
                    JERV_LOG_INFO("{}(Double) {}", pad, std::get<double>(value));
                    break;
                case String:
                    JERV_LOG_INFO("{}(String) \"{}\"", pad, std::get<std::string>(value));
                    break;
                case Int8List: {
                    break;
                }
                case Int32List: {
                    break;
                }
                case Int64List: {
                    break;
                }
                case List: {
                    auto &list = std::get<std::vector<NBTData> >(value);
                    JERV_LOG_INFO("{}(List) [{} entries]", pad, list.size());
                    for (size_t i = 0; i < list.size(); ++i) {
                        JERV_LOG_INFO("{}  [{}] ", pad, i);
                        list[i].print(indent + 2);
                    }
                    break;
                }
                case Compound: {
                    auto &map = std::get<std::unordered_map<std::string, NBTData> >(value);
                    JERV_LOG_INFO("{}(Compound) {{{} entries}}", pad, map.size());
                    for (auto &[key, val]: map) {
                        JERV_LOG_INFO("{}  \"{}\": ", pad, key);
                        val.print(indent + 2);
                    }
                    break;
                }
            }
        }
    };

    class NBT {
    public:
        static NBT create(const std::span<uint8_t> buffer) {
            return NBT(buffer);
        }

        static NBT create(std::vector<uint8_t> &buffer) {
            return NBT(std::span(buffer));
        }

        explicit NBT(const std::span<uint8_t> buffer)
            : cursor_(buffer) {
            // 8 bcs of nbt header, should be checked for properly in the future, bcs some nbt's don't have the header
            cursor_.setPointer(8);
        }

        bool isEnd() const {
            return cursor_.isEndOfStream();
        }

        NBTData next() {
            uint8_t type = cursor_.readUint8();
            if (type == NBTDataType::EndOfCompound) {
                return {NBTDataType::EndOfCompound, int8_t{0}};
            }
            std::string name = cursor_.readStringLE16();
            JERV_LOG_INFO("Root tag: {} type: {}", name,  static_cast<int>(type));
            auto reader = getReader(static_cast<NBTDataType>(type));
            return reader(*this, static_cast<NBTDataType>(type));
        }

    private:
        using ReaderFn = NBTData(*)(NBT &, NBTDataType);

        static ReaderFn getReader(NBTDataType type) {
            static constexpr ReaderFn table[] = {
                // 0x00 EndOfCompound
                [](NBT &n, NBTDataType t) -> NBTData { return {t, int8_t{0}}; },
                // 0x01 Int8
                [](NBT &n, NBTDataType t) -> NBTData { return {t, n.cursor_.readInt8()}; },
                // 0x02 Int16
                [](NBT &n, NBTDataType t) -> NBTData { return {t, n.cursor_.readInt16<true>()}; },
                // 0x03 Int32
                [](NBT &n, NBTDataType t) -> NBTData { return {t, n.cursor_.readInt32<true>()}; },
                // 0x04 Int64
                [](NBT &n, NBTDataType t) -> NBTData { return {t, n.cursor_.readInt64<true>()}; },
                // 0x05 Float
                [](NBT &n, NBTDataType t) -> NBTData { return {t, n.cursor_.readFloat32<true>()}; },
                // 0x06 Double
                [](NBT &n, NBTDataType t) -> NBTData { return {t, n.cursor_.readFloat64<true>()}; },
                // 0x07 Int8List
                [](NBT &n, NBTDataType t) -> NBTData {
                },
                // 0x08 String
                [](NBT &n, NBTDataType t) -> NBTData { return {t, n.cursor_.readStringLE16()}; },
                // 0x09 List
                [](NBT &n, NBTDataType t) -> NBTData {
                    uint8_t listType = n.cursor_.readUint8();
                    int32_t size = n.cursor_.readInt32<true>();
                    std::vector<NBTData> list;
                    list.reserve(size);
                    auto reader = getReader(static_cast<NBTDataType>(listType));
                    for (int32_t i = 0; i < size; ++i) {
                        list.push_back(reader(n, static_cast<NBTDataType>(listType)));
                    }
                    return {t, std::move(list)};
                },
                // 0x0A Compound
                [](NBT &n, NBTDataType t) -> NBTData {
                    std::unordered_map<std::string, NBTData> map;
                    while (true) {
                        uint8_t childType = n.cursor_.readUint8();
                        if (childType == NBTDataType::EndOfCompound) break;
                        std::string name = n.cursor_.readStringLE16();
                        auto reader = getReader(static_cast<NBTDataType>(childType));
                        map.emplace(std::move(name), reader(n, static_cast<NBTDataType>(childType)));
                    }
                    return {t, std::move(map)};
                },
                // 0x0B Int32List
                [](NBT &n, NBTDataType t) -> NBTData {
                },
                // 0x0C Int64List
                [](NBT &n, NBTDataType t) -> NBTData {
                },
            };
            return table[static_cast<uint8_t>(type)];
        }

        jerv::binary::Cursor cursor_;
    };
}
