#pragma once

#include <jerv/protocol/packet.hpp>
#include <string>
#include <vector>

namespace jerv::protocol {
    enum TextType : uint8_t {
        Raw = 0,
        Chat = 1,
        Translation = 2,
        Popup = 3,
        JukeboxPopup = 4,
        Tip = 5,
        System = 6,
        Whisper = 7,
        Announcement = 8,
        Json = 9,
        JsonWhisper = 10,
        JsonAnnouncement = 11
    };

    class TextPacket : public PacketType {
    public:
        TextType type = TextType::Raw;
        bool needsTranslation = false;

        std::string sourceName;
        std::string message;
        std::vector<std::string> parameters;

        std::string xuid;
        std::string platformChatId;
        std::string filteredMessage;

        PacketId getPacketId() const override {
            return PacketId::Text;
        }

        void serialize(binary::cursor &cursor) const override {
            cursor.writeBool(needsTranslation);

            const auto normalizeMessage = [](const std::string &text) {
                return text.empty() ? std::string(" ") : text;
            };

            switch (type) {
                case TextType::Raw:
                case TextType::Tip:
                case TextType::System:
                case TextType::Json:
                case TextType::JsonWhisper:
                case TextType::JsonAnnouncement: {
                    static const char *messageOnlyKeys[] = {
                        "raw", "tip", "systemMessage", "textObjectWhisper", "textObjectAnnouncement", "textObject"
                    };

                    cursor.writeUint8(0);
                    for (const auto *key: messageOnlyKeys) {
                        cursor.writeString(key);
                    }
                    cursor.writeUint8(type);
                    cursor.writeString(normalizeMessage(message));
                    break;
                }
                case TextType::Chat:
                case TextType::Whisper:
                case TextType::Announcement: {
                    static const char *authorKeys[] = {"chat", "whisper", "announcement"};

                    cursor.writeUint8(1);
                    for (const auto *key: authorKeys) {
                        cursor.writeString(key);
                    }
                    cursor.writeUint8(type);
                    cursor.writeString(sourceName);
                    cursor.writeString(normalizeMessage(message));
                    break;
                }
                case TextType::Translation:
                case TextType::Popup:
                case TextType::JukeboxPopup: {
                    static const char *paramKeys[] = {"translate", "popup", "jukeboxPopup"};

                    cursor.writeUint8(2);
                    for (const auto *key: paramKeys) {
                        cursor.writeString(key);
                    }
                    cursor.writeUint8(type);
                    cursor.writeString(normalizeMessage(message));
                    cursor.writeVarInt32(static_cast<int32_t>(parameters.size()));
                    for (const auto &param: parameters) {
                        cursor.writeString(param);
                    }
                    break;
                }
                default:
                    break;
            }

            cursor.writeString(xuid);
            cursor.writeString(platformChatId);

            const bool hasFilteredMessage = !filteredMessage.empty();
            cursor.writeBool(hasFilteredMessage);
            if (hasFilteredMessage) {
                cursor.writeString(filteredMessage);
            }
        }

        void deserialize(binary::cursor &cursor) override {
            needsTranslation = cursor.readBool();
            const uint8_t messageFormat = cursor.readUint8();

            switch (messageFormat) {
                case 0: {
                    for (int i = 0; i < 6; i++) {
                        cursor.readString();
                    }

                    type = static_cast<TextType>(cursor.readUint8());
                    message = cursor.readString();
                    break;
                }
                case 1: {
                    for (int i = 0; i < 3; i++) {
                        cursor.readString();
                    }

                    type = static_cast<TextType>(cursor.readUint8());
                    sourceName = cursor.readString();
                    message = cursor.readString();
                    break;
                }
                case 2: {
                    for (int i = 0; i < 3; i++) {
                        cursor.readString();
                    }

                    type = static_cast<TextType>(cursor.readUint8());
                    message = cursor.readString();

                    const int32_t paramCount = cursor.readVarInt32();
                    parameters.reserve(paramCount);
                    for (int32_t i = 0; i < paramCount; i++) {
                        parameters.emplace_back(cursor.readString());
                    }

                    break;
                }
                default:
                    break;
            }

            xuid = cursor.readString();
            platformChatId = cursor.readString();

            const bool hasFilteredMessage = cursor.readBool();
            if (hasFilteredMessage) {
                filteredMessage = cursor.readString();
            } else {
                filteredMessage.clear();
            }
        }
    };
}


