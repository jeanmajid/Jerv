#pragma once

#include <jerv/protocol/packet.hpp>
#include <string>
#include <vector>

namespace jerv::protocol {
    enum TextContentType : uint32_t {
        MessageOnly = 0,
        AuthorAndMessage = 1,
        MessageAndParams = 2
    };

    enum TextTypeMessageOnly : uint8_t {
        Raw = 0,
        Tip = 1,
        SystemMessage = 2,
        TextObjectWhisper = 3,
        TextObject = 4,
        TextObjectAnnouncment = 5
    };

    enum TextTypeAuthorAndMessage : uint8_t {
        Chat = 0,
        Whisper = 1,
        Announcment = 2
    };

    enum TextTypeMessageAndParams : uint8_t {
        Translate = 0,
        Popup = 1,
        JukeboxPopup = 2
    };

    class TextPacket : public PacketType {
    public:
        bool localize = false;
        TextContentType contentType;
        uint8_t messageType;

        std::string playerName;
        std::string message;
        std::vector<std::string> parameters;

        std::string xuid;
        std::string platformChatId;
        std::string filteredMessage;

        PacketId getPacketId() const override {
            return PacketId::Text;
        }

        void serialize(binary::Cursor &cursor) const override {
            cursor.writeBool(localize);
            cursor.writeVarInt32(contentType);

            switch (contentType) {
                case TextContentType::MessageOnly: {
                    cursor.writeUint8(messageType);
                    cursor.writeString(message);
                    break;
                }
                case TextContentType::AuthorAndMessage: {
                    cursor.writeUint8(messageType);
                    cursor.writeString(playerName);
                    cursor.writeString(message);
                    break;
                }
                case TextContentType::MessageAndParams: {
                    cursor.writeUint8(messageType);
                    cursor.writeString(message);
                    cursor.writeVarInt32(static_cast<int32_t>(parameters.size()));
                    for (const auto &param: parameters) {
                        cursor.writeString(param);
                    }
                    break;
                }
            }

            cursor.writeString(xuid);
            cursor.writeString(platformChatId);

            const bool hasFilteredMessage = !filteredMessage.empty();
            cursor.writeBool(hasFilteredMessage);
            if (hasFilteredMessage) {
                cursor.writeString(filteredMessage);
            }
        }

        void deserialize(binary::Cursor &cursor) override {
            localize = cursor.readBool();
            contentType = static_cast<TextContentType>(cursor.readVarInt32());

            switch (contentType) {
                case TextContentType::MessageOnly: {
                    messageType = cursor.readUint8();
                    message = cursor.readString();
                    break;
                }
                case TextContentType::AuthorAndMessage: {
                    messageType = cursor.readUint8();
                    playerName = cursor.readString();
                    message = cursor.readString();
                    break;
                }
                case TextContentType::MessageAndParams: {
                    messageType = cursor.readUint8();
                    message = cursor.readString();
                    int32_t length = cursor.readVarInt32();
                    parameters.reserve(length);
                    for (int i = 0; i < length; ++i) {
                        parameters.emplace_back(cursor.readString());
                    }
                    break;
                }
            }

            xuid = cursor.readString();
            platformChatId = cursor.readString();

            bool hasFilteredMessage = cursor.readBool();
            if (hasFilteredMessage) {
                filteredMessage = cursor.readString();
            }
        }
    };
}
