#pragma once

#include <jerv/protocol/packet.hpp>
#include <jerv/protocol/enums.hpp>
#include <string>
#include <vector>
#include <variant>

namespace jerv::protocol {
    enum GameMode : int32_t {
        Survival = 0,
        Creative = 1,
        Adventure = 2,
        SurvivalSpectator = 3,
        CreativeSpectator = 4,
        Fallback = 5,
        Spectator = 6
    };

    enum GeneratorType : int32_t {
        OldLimited = 0,
        Infinite = 1,
        Flat = 2,
        Nether = 3,
        End = 4
    };

    enum Difficulty : int32_t {
        Peaceful = 0,
        Easy = 1,
        Normal = 2,
        Hard = 3
    };

    enum EditorWorldType : int32_t {
        NotEditor = 0,
        Project = 1,
        TestLevel = 2,
        RealmsUpload = 3
    };

    enum PermissionLevel : int32_t {
        Visitor = 0,
        Member = 1,
        Operator = 2,
        Custom = 3
    };

    enum ChatRestrictionLevel : uint8_t {
        None = 0,
        Dropped = 1,
        Disabled = 2
    };

    enum GameRuleType : int32_t {
        Bool = 1,
        Int = 2,
        Float = 3
    };

    struct GameRule {
        std::string name;
        bool editable = true;
        GameRuleType type = GameRuleType::Bool;
        std::variant<bool, int32_t, float> value;
    };

    struct EducationSharedResourceURI {
        std::string buttonName;
        std::string linkUri;
    };

    struct Experiments {
        std::vector<std::pair<std::string, bool> > experiments;
    };

    struct BlockProperty {
        std::string name;
        std::vector<uint8_t> stateNbt;
    };

    class StartGamePacket : public PacketType {
    public:
        int64_t entityId = 0;
        uint64_t runtimeEntityId = 0;

        GameMode playerGameMode = GameMode::Survival;
        Vec3f playerPosition;
        Vec2f rotation;

        uint64_t seed = 0;
        int16_t biomeType = 0;
        std::string biomeName;
        DimensionId dimension = DimensionId::Overworld;
        GeneratorType generator = GeneratorType::Infinite;

        GameMode worldGameMode = GameMode::Survival;
        bool hardcore = false;
        Difficulty difficulty = Difficulty::Normal;
        BlockLocation spawnPosition;

        bool achievementsDisabled = false;
        EditorWorldType editorWorldType = EditorWorldType::NotEditor;
        bool createdInEditor = false;
        bool exportedFromEditor = false;

        int32_t dayCycleStopTime = 0;

        int32_t eduOffer = 0;
        bool eduFeaturesEnabled = false;
        std::string eduProductUuid;

        float rainLevel = 0.0f;
        float lightningLevel = 0.0f;

        bool hasConfirmedPlatformLockedContent = false;
        bool isMultiplayer = true;
        bool broadcastToLan = true;
        int32_t xboxLiveBroadcastMode = 0;
        int32_t platformBroadcastMode = 0;

        bool enableCommands = true;
        bool isTexturepacksRequired = false;

        std::vector<GameRule> gameRules;

        Experiments experiments;
        bool experimentsPreviouslyUsed = false;

        bool bonusChest = false;
        bool mapEnabled = false;
        PermissionLevel permissionLevel = PermissionLevel::Member;
        int32_t serverChunkTickRange = 4;

        bool hasLockedBehaviorPack = false;
        bool hasLockedResourcePack = false;
        bool isFromLockedWorldTemplate = false;
        bool msaGamertasOnly = false;
        bool isFromWorldTemplate = false;
        bool isWorldTemplateOptionLocked = false;
        bool onlySpawnV1Villagers = false;

        bool personaDisabled = false;
        bool customSkinsDisabled = false;
        bool emoteChatMuted = false;

        std::string gameVersion = "*";
        int32_t limitedWorldWidth = 0;
        int32_t limitedWorldLength = 0;
        bool isNewNether = true;

        EducationSharedResourceURI eduResourceUri;
        bool experimentalGameplayOverride = false;

        ChatRestrictionLevel chatRestrictionLevel = ChatRestrictionLevel::None;
        bool disablePlayerInteractions = false;

        std::string serverIdentifier;
        std::string worldIdentifier;
        std::string scenarioIdentifier;
        std::string ownerIdentifier;
        std::string levelId;
        std::string worldName;
        std::string premiumWorldTemplateId;

        bool isTrial = false;
        int32_t rewindHistorySize = 0;
        bool serverAuthoritativeBlockBreaking = false;

        int64_t currentTick = 0;
        int32_t enchantmentSeed = 0;

        std::vector<BlockProperty> blockProperties;

        std::string multiplayerCorrelationId;
        bool serverAuthoritativeInventory = true;
        std::string engine = "Jerver";

        std::vector<uint8_t> propertyData;

        uint64_t blockPaletteChecksum = 0;

        std::array<uint8_t, 16> worldTemplateId = {};

        bool clientSideGeneration = false;
        bool blockNetworkIdsAreHashes = true;
        bool serverControlledSound = true;

        PacketId getPacketId() const override {
            return PacketId::StartGame;
        }

        void serialize(binary::cursor &cursor) const override {
            cursor.writeZigZag64(entityId);
            writeVarInt64(cursor, runtimeEntityId);

            cursor.writeZigZag32(playerGameMode);
            playerPosition.serialize(cursor);
            rotation.serialize(cursor);

            cursor.writeUint64<true>(seed);
            cursor.writeInt16<true>(biomeType);
            cursor.writeString(biomeName);
            cursor.writeZigZag32(static_cast<int32_t>(dimension));
            cursor.writeZigZag32(generator);

            cursor.writeZigZag32(worldGameMode);
            cursor.writeBool(hardcore);
            cursor.writeZigZag32(difficulty);
            spawnPosition.serialize(cursor);

            cursor.writeBool(achievementsDisabled);
            cursor.writeZigZag32(editorWorldType);
            cursor.writeBool(createdInEditor);
            cursor.writeBool(exportedFromEditor);

            cursor.writeZigZag32(dayCycleStopTime);

            cursor.writeZigZag32(eduOffer);
            cursor.writeBool(eduFeaturesEnabled);
            cursor.writeString(eduProductUuid);

            cursor.writeFloat32<true>(rainLevel);
            cursor.writeFloat32<true>(lightningLevel);

            cursor.writeBool(hasConfirmedPlatformLockedContent);
            cursor.writeBool(isMultiplayer);
            cursor.writeBool(broadcastToLan);
            cursor.writeVarInt32(xboxLiveBroadcastMode);
            cursor.writeVarInt32(platformBroadcastMode);

            cursor.writeBool(enableCommands);
            cursor.writeBool(isTexturepacksRequired);

            cursor.writeVarInt32(static_cast<int32_t>(gameRules.size()));
            for (const auto &rule: gameRules) {
                cursor.writeString(rule.name);
                cursor.writeBool(rule.editable);
                cursor.writeVarInt32(rule.type);
                switch (rule.type) {
                    case GameRuleType::Bool:
                        cursor.writeBool(std::get<bool>(rule.value));
                        break;
                    case GameRuleType::Int:
                        cursor.writeZigZag32(std::get<int32_t>(rule.value));
                        break;
                    case GameRuleType::Float:
                        cursor.writeFloat32<true>(std::get<float>(rule.value));
                        break;
                }
            }


            cursor.writeInt32<true>(static_cast<int32_t>(experiments.experiments.size()));
            for (const auto &exp: experiments.experiments) {
                cursor.writeString(exp.first);
                cursor.writeBool(exp.second);
            }
            cursor.writeBool(experimentsPreviouslyUsed);

            cursor.writeBool(bonusChest);
            cursor.writeBool(mapEnabled);
            cursor.writeUint8(static_cast<uint8_t>(permissionLevel));
            cursor.writeInt32<true>(serverChunkTickRange);

            cursor.writeBool(hasLockedBehaviorPack);
            cursor.writeBool(hasLockedResourcePack);
            cursor.writeBool(isFromLockedWorldTemplate);
            cursor.writeBool(msaGamertasOnly);
            cursor.writeBool(isFromWorldTemplate);
            cursor.writeBool(isWorldTemplateOptionLocked);
            cursor.writeBool(onlySpawnV1Villagers);

            cursor.writeBool(personaDisabled);
            cursor.writeBool(customSkinsDisabled);
            cursor.writeBool(emoteChatMuted);

            cursor.writeString(gameVersion);
            cursor.writeInt32<true>(limitedWorldWidth);
            cursor.writeInt32<true>(limitedWorldLength);
            cursor.writeBool(isNewNether);

            cursor.writeString(eduResourceUri.buttonName);
            cursor.writeString(eduResourceUri.linkUri);
            cursor.writeBool(experimentalGameplayOverride);

            cursor.writeUint8(static_cast<uint8_t>(chatRestrictionLevel));
            cursor.writeBool(disablePlayerInteractions);

            cursor.writeString(serverIdentifier);
            cursor.writeString(worldIdentifier);
            cursor.writeString(scenarioIdentifier);
            cursor.writeString(ownerIdentifier);
            cursor.writeString(levelId);
            cursor.writeString(worldName);
            cursor.writeString(premiumWorldTemplateId);

            cursor.writeBool(isTrial);
            cursor.writeZigZag32(rewindHistorySize);
            cursor.writeBool(serverAuthoritativeBlockBreaking);

            cursor.writeInt64<true>(currentTick);
            cursor.writeZigZag32(enchantmentSeed);

            cursor.writeVarInt32(static_cast<int32_t>(blockProperties.size()));
            for (const auto &prop: blockProperties) {
                cursor.writeString(prop.name);
                cursor.writeSliceSpan(prop.stateNbt);
            }

            cursor.writeString(multiplayerCorrelationId);
            cursor.writeBool(serverAuthoritativeInventory);
            cursor.writeString(engine);

            if (propertyData.empty()) {
                cursor.writeUint8(0x0A);
                cursor.writeUint8(0x00);
                cursor.writeUint8(0x00);
            } else {
                cursor.writeSliceSpan(propertyData);
            }

            cursor.writeUint64<true>(blockPaletteChecksum);

            cursor.writeSliceSpan(std::span(worldTemplateId.data(), 16));

            cursor.writeBool(clientSideGeneration);
            cursor.writeBool(blockNetworkIdsAreHashes);
            cursor.writeBool(serverControlledSound);
        }

        void deserialize(binary::cursor &cursor) override {
        }

    private:
        static void writeVarInt64(binary::cursor &cursor, uint64_t value) {
            while (value >= 0x80) {
                cursor.writeUint8(static_cast<uint8_t>((value & 0x7F) | 0x80));
                value >>= 7;
            }
            cursor.writeUint8(static_cast<uint8_t>(value));
        }
    };
}