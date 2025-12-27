#pragma once

#include <jerv/protocol/packet.hpp>

namespace jerv::protocol {
    class AvailableCommandsPacket : public PacketType {
    public:
        struct CommandEnum {
            size_t enumValuesSize = 0;
            std::string name;
            std::vector<uint32_t> values;

            void serialize(binary::cursor &cursor) const {
                cursor.writeString(name);
                cursor.writeVarInt(values.size());
                for (const uint32_t value: values) {
                    cursor.writeUint32(value, true);
                }
            }
        };

        struct ChainedSubcommandValue {
            uint16_t index;
            uint16_t value;
        };

        struct ChainedSubcommand {
            std::string name;
            std::vector<ChainedSubcommandValue> values;

            void serialize(binary::cursor &cursor) const {
                cursor.writeString(name);

                cursor.writeVarInt(values.size());
                for (const ChainedSubcommandValue &value: values) {
                    cursor.writeUint16(value.index, true);
                    cursor.writeUint16(value.value, true);
                }
            }
        };

        enum CommandValueType : uint16_t {
            Int = 1,
            Float = 3,
            Value = 4,
            WildcardInt = 5,
            Operator = 6,
            CompareOperator = 7,
            Target = 8,
            WildcardTarget = 10,
            FilePath = 17,
            IntegerRange = 23,
            EquipmentSlots = 47,
            String = 48,
            BlockPosition = 64,
            Position = 65,
            Message = 67,
            RawText = 70,
            Json = 74,
            BlockStates = 84,
            Command = 87,
        };

        enum CommandEnumType : uint16_t {
            Enum = 16,
            EnumType = 48,
            Suffixed = 256,
            SoftEnum = 1040
        };

        struct DynamicEnum {
            std::string name;
            std::vector<std::string> values;

            void serialize(binary::cursor &cursor) const {
                cursor.writeString(name);
                cursor.writeVarInt(values.size());
                for (const std::string &value: values) {
                    cursor.writeString(value);
                }
            }
        };

        struct EnumConstraint {
            uint32_t valueIndex;
            uint32_t enumIndex;
            std::vector<uint8_t> constraints;

            void serialize(binary::cursor &cursor) const {
                cursor.writeUint32(valueIndex, true);
                cursor.writeUint32(enumIndex, true);

                cursor.writeVarInt(constraints.size());
                for (const uint8_t value: constraints) {
                    cursor.writeUint8(value);
                }
            }
        };

        struct Parameter {
            std::string parameterName;
            CommandValueType commandValueType;
            CommandEnumType commandEnumType;
            bool optional;
            uint8_t options;

            void serialize(binary::cursor &cursor) const {
                cursor.writeString(parameterName);
                cursor.writeUint32(static_cast<uint32_t>(commandEnumType) << 16 | static_cast<uint32_t>(commandValueType), true);
                cursor.writeBool(optional);
                cursor.writeUint8(options);
            }
        };

        struct Overload {
            bool chaining;
            std::vector<Parameter> parameters;

            void serialize(binary::cursor &cursor) const {
                cursor.writeBool(chaining);

                cursor.writeVarInt(parameters.size());
                for (const Parameter &parameter: parameters) {
                    parameter.serialize(cursor);
                }
            }
        };

        struct CommandData {
            std::string name;
            std::string description;
            uint16_t flags;
            std::string permissionLevel;
            int32_t alias;
            std::vector<uint16_t> chainedSubcommandOffsets;
            std::vector<Overload> overloads;

            void serialize(binary::cursor &cursor) const {
                cursor.writeString(name);
                cursor.writeString(description);
                cursor.writeUint16(flags, true);
                cursor.writeString(permissionLevel);
                cursor.writeInt32(alias, true);

                cursor.writeVarInt(chainedSubcommandOffsets.size());
                for (const uint16_t &chainedSubcommandOffset: chainedSubcommandOffsets) {
                    cursor.writeUint16(chainedSubcommandOffset, true);
                }

                cursor.writeVarInt(overloads.size());
                for (const Overload &overload: overloads) {
                    overload.serialize(cursor);
                }
            }
        };

        std::vector<std::string> enumValues;
        std::vector<std::string> chainedSubcommandValues;
        std::vector<std::string> suffixes;
        std::vector<CommandEnum> enums;
        std::vector<ChainedSubcommand> chainedSubcommands;
        std::vector<CommandData> commandData;
        std::vector<DynamicEnum> dynamicEnums;
        std::vector<EnumConstraint> enumConstraints;

        PacketId getPacketId() const override {
            return PacketId::AvailableCommands;
        }

        void serialize(binary::cursor &cursor) const override {
            cursor.writeVarInt(enumValues.size());
            for (const std::string &string: enumValues) {
                cursor.writeString(string);
            }

            cursor.writeVarInt(chainedSubcommandValues.size());
            for (const std::string &string: chainedSubcommandValues) {
                cursor.writeString(string);
            }

            cursor.writeVarInt(suffixes.size());
            for (const std::string &string: suffixes) {
                cursor.writeString(string);
            }

            cursor.writeVarInt(enums.size());
            for (const CommandEnum &commandEnum: enums) {
                commandEnum.serialize(cursor);
            }

            cursor.writeVarInt(chainedSubcommands.size());
            for (const ChainedSubcommand &chainedSubcommand: chainedSubcommands) {
                chainedSubcommand.serialize(cursor);
            }

            cursor.writeVarInt(commandData.size());
            for (const CommandData &command: commandData) {
                command.serialize(cursor);
            }

            cursor.writeVarInt(dynamicEnums.size());
            for (const DynamicEnum &dynamicEnum: dynamicEnums) {
                dynamicEnum.serialize(cursor);
            }

            cursor.writeVarInt(enumConstraints.size());
            for (const EnumConstraint &constraint: enumConstraints) {
                constraint.serialize(cursor);
            }
        }

        void deserialize(binary::cursor &cursor) override {
        }
    };
}
