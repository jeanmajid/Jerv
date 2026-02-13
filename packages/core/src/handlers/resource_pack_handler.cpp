#include <jerv/core/network_connection.hpp>
#include <jerv/protocol/enums.hpp>
#include <jerv/common/logger.hpp>

#include "jerv/protocol/packets/set_actor_data.hpp"

namespace jerv::core {
    void NetworkConnection::handleResourcePackClientResponse(binary::cursor &cursor) {
        protocol::ResourcePackClientResponsePacket packet;
        packet.deserialize(cursor);

        switch (packet.response) {
            case protocol::ResourcePackResponse::Refused: {
                break;
            }

            case protocol::ResourcePackResponse::SendPacks: {
                break;
            }

            case protocol::ResourcePackResponse::HaveAllPacks: {
                protocol::ResourcePackStackPacket stack;
                stack.mustAccept = false;
                stack.texturePacks = {};
                stack.gameVersion = "*";
                stack.experiments = {};
                stack.experimentsPreviouslyToggled = false;
                stack.hasEditorPacks = false;
                send(stack);
                break;
            }

            case protocol::ResourcePackResponse::Completed: {
                protocol::StartGamePacket startGame;
                startGame.entityId = 1;
                startGame.runtimeEntityId = 1;
                startGame.playerGameMode = protocol::GameMode::Survival;
                startGame.playerPosition = {0.0f, 128.0f, 0.0f};
                startGame.rotation = {0.0f, 0.0f};
                startGame.seed = 12345;
                startGame.biomeType = 0;
                startGame.biomeName = "plains";
                startGame.dimension = protocol::DimensionId::Overworld;
                startGame.generator = protocol::GeneratorType::Infinite;
                startGame.worldGameMode = protocol::GameMode::Creative;
                startGame.hardcore = false;
                startGame.difficulty = protocol::Difficulty::Normal;
                startGame.spawnPosition = {0, 64, 0};
                startGame.achievementsDisabled = true;
                startGame.editorWorldType = protocol::EditorWorldType::NotEditor;
                startGame.createdInEditor = false;
                startGame.exportedFromEditor = false;
                startGame.dayCycleStopTime = 0;
                startGame.eduOffer = 0;
                startGame.eduFeaturesEnabled = false;
                startGame.eduProductUuid = "";
                startGame.rainLevel = 0.0f;
                startGame.lightningLevel = 0.0f;
                startGame.hasConfirmedPlatformLockedContent = false;
                startGame.isMultiplayer = true;
                startGame.broadcastToLan = true;
                startGame.xboxLiveBroadcastMode = 6;
                startGame.platformBroadcastMode = 6;
                startGame.enableCommands = true;
                startGame.isTexturepacksRequired = false;

                protocol::GameRule showCoordsRule;
                showCoordsRule.name = "showcoordinates";
                showCoordsRule.type = protocol::GameRuleType::Bool;
                showCoordsRule.value = true;

                startGame.gameRules = {showCoordsRule};
                startGame.experiments = {};
                startGame.experimentsPreviouslyUsed = false;
                startGame.bonusChest = false;
                startGame.mapEnabled = false;
                startGame.permissionLevel = protocol::PermissionLevel::Operator;
                startGame.serverChunkTickRange = 4;
                startGame.hasLockedBehaviorPack = false;
                startGame.hasLockedResourcePack = false;
                startGame.isFromLockedWorldTemplate = false;
                startGame.msaGamertasOnly = false;
                startGame.isFromWorldTemplate = false;
                startGame.isWorldTemplateOptionLocked = false;
                startGame.onlySpawnV1Villagers = false;
                startGame.personaDisabled = false;
                startGame.customSkinsDisabled = false;
                startGame.emoteChatMuted = false;
                startGame.gameVersion = "*";
                startGame.limitedWorldWidth = 16;
                startGame.limitedWorldLength = 16;
                startGame.isNewNether = false;
                startGame.eduResourceUri = {"", ""};
                startGame.experimentalGameplayOverride = false;
                startGame.chatRestrictionLevel = protocol::ChatRestrictionLevel::None;
                startGame.disablePlayerInteractions = false;
                startGame.levelId = "Jerver";
                startGame.worldName = "Jerver World";
                startGame.premiumWorldTemplateId = "";
                startGame.isTrial = false;
                startGame.rewindHistorySize = 0;
                startGame.serverAuthoritativeBlockBreaking = true;
                startGame.currentTick = 0;
                startGame.enchantmentSeed = 12345;
                startGame.blockProperties = {};
                startGame.multiplayerCorrelationId = "<raknet>a555-7ece-2f1c-8f69";
                startGame.serverAuthoritativeInventory = true;
                startGame.engine = "Jerver";
                startGame.propertyData = {};
                startGame.blockPaletteChecksum = 0;
                startGame.worldTemplateId = {};
                startGame.clientSideGeneration = false;
                startGame.blockNetworkIdsAreHashes = true;
                startGame.serverControlledSound = true;
                startGame.experienceId = "Jerver";
                startGame.experienceWorldId = "Jerver";
                startGame.experienceName = "Jerver";
                startGame.experienceCreatorId = "";

                protocol::AvailableActorIdentifiersPacket actors;

                protocol::ItemRegistryPacket itemRegistry;
                itemRegistry.definitions = {};

                protocol::BiomeDefinitionListPacket biomeList;
                biomeList.definitions = {};
                biomeList.identifiers = {};

                protocol::CreativeContentPacket creativeContent;
                creativeContent.groups = {};
                creativeContent.items = {};

                protocol::CraftingDataPacket craftingData;
                craftingData.recipes = {};
                craftingData.potionMixData = {};
                craftingData.containerMixData = {};
                craftingData.materialReducers = {};
                craftingData.clearRecipes = true;

                protocol::AvailableCommandsPacket availableCommands;
                availableCommands.enumValues = {};
                availableCommands.chainedSubcommandValues = {};
                availableCommands.suffixes = {};
                availableCommands.enums = {};
                availableCommands.chainedSubcommands = {};
                availableCommands.dynamicEnums = {};
                availableCommands.enumConstraints = {};

                protocol::AvailableCommandsPacket::CommandData commandData;
                commandData.name = "tp";
                commandData.description = "teleport somewhere";
                commandData.flags = 0;
                commandData.permissionLevel = "Any";
                commandData.alias = -1;

                protocol::AvailableCommandsPacket::Parameter parameter;
                parameter.parameterName = "location";
                parameter.commandValueType = protocol::AvailableCommandsPacket::CommandValueType::Position;
                parameter.commandEnumType = protocol::AvailableCommandsPacket::CommandEnumType::Enum;
                parameter.optional = false;
                parameter.options = 0;

                protocol::AvailableCommandsPacket::Overload overload;
                overload.chaining = false;
                overload.parameters = {parameter};

                commandData.overloads = {overload};
                availableCommands.commandData = {commandData};

                sendImmediate(startGame, actors, itemRegistry, biomeList, creativeContent, craftingData,
                              availableCommands);

                protocol::SetActorDataPacket::MetaDataDictionary flags;
                flags.key = 0;
                flags.type = protocol::SetActorDataPacket::MetaDataDictionaryType::Long;
                flags.value = 1LL << 49 | 1LL << 35;

                protocol::SetActorDataPacket::MetaDataDictionary longExtended;
                longExtended.key = 92;
                longExtended.type = protocol::SetActorDataPacket::MetaDataDictionaryType::Long;
                longExtended.value = 0LL;

                protocol::SetActorDataPacket setActorData;
                setActorData.runtimeEntityId = 1;
                setActorData.metaData = {flags, longExtended};

                send(setActorData);
                break;
            }
            default:
                break;
        }
    }
}
