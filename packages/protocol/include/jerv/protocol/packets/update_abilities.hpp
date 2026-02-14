#pragma once

#include <jerv/protocol/packet.hpp>
#include "jerv/protocol/enums.hpp"

namespace jerv::protocol {
    enum class CommandPermissionLevel : uint8_t {
        Normal = 0,
        Operator = 1,
        Automation = 2,
        Host = 3,
        Owner = 4,
        Internal = 5
    };

    enum class AbilityLayerType : uint16_t {
        Cache = 0,
        Base = 1,
        Spectator = 2,
        Commands = 3,
        Editor = 4,
        LoadingScreen = 5
    };

    struct AbilityLayer {
        AbilityLayerType type;
        uint32_t allowedAbilities;
        uint32_t enabledAbilities;
        float flySpeed;
        float verticalFlySpeed;
        float walkSpeed;

        void serialize(binary::cursor &cursor) const {
            cursor.writeUint16<true>(static_cast<uint16_t>(type));
            cursor.writeUint32<true>(allowedAbilities);
            cursor.writeUint32<true>(enabledAbilities);
            cursor.writeFloat32<true>(flySpeed);
            cursor.writeFloat32<true>(verticalFlySpeed);
            cursor.writeFloat32<true>(walkSpeed);
        }

        void deserialize(binary::cursor &cursor) {

        }
    };

    class UpdateAbilitiesPacket : public PacketType {
    public:
        int64_t entityUniqueId;
        PermissionLevel permissionLevel;
        CommandPermissionLevel commandPermissionLevel;
        std::vector<AbilityLayer> abilityLayers;

        PacketId getPacketId() const override {
            return PacketId::UpdateAbilities;
        }

        void serialize(binary::cursor &cursor) const override {
            cursor.writeInt64<true>(entityUniqueId);
            cursor.writeUint8(permissionLevel);
            cursor.writeUint8(static_cast<uint8_t>(commandPermissionLevel));

            cursor.writeUint8(abilityLayers.size());
            for (const AbilityLayer& ability : abilityLayers) {
                ability.serialize(cursor);
            }
        }

        void deserialize(binary::cursor &cursor) override {

        }
    };
}


