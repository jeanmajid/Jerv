#pragma once

#include <jerv/protocol/packet.hpp>
#include <jerv/binary/cursor.hpp>
#include <nlohmann/json.hpp>
#include <vector>
#include <string>
#include <optional>
#include <algorithm>
#include <cstring>

#include "jerv/common/logger.hpp"

namespace jerv::protocol {
    struct PlayerIdentity {
        std::string displayName;
        std::string xuid;
        std::string uuid;
        std::string titleId;
        std::string identityPublicKey;
    };

    struct MicrosoftAuthToken {
        std::string xboxGamertag;
        std::string xboxUid;
        std::string playFabId;
        std::string issuer;
        std::string audience;
    };

    struct ClientData {
        std::string selfSignedId;
        std::string serverAddress;
        int64_t clientRandomId = 0;

        std::string deviceModel;
        int deviceOS = 0;
        std::string deviceId;
        std::string gameVersion;

        std::string languageCode;
        int guiScale = 0;
        int uiProfile = 0;

        int defaultInputMode = 0;
        int currentInputMode = 0;

        std::string skinId;
        int skinImageWidth = 0;
        int skinImageHeight = 0;
        std::string skinData;
        std::string skinResourcePatch;
        std::string skinGeometryData;

        std::string platformUserId;
        std::string thirdPartyName;
        std::string playFabId;

        bool trustedSkin = false;
        bool premiumSkin = false;
        bool personaSkin = false;
        bool capeOnClassicSkin = false;
        std::string capeId;

        bool compatibleWithClientSideChunkGen = false;
        bool isEditorMode = false;
    };

    namespace detail {
        inline std::string base64UrlDecode(const std::string &input) {
            std::string base64 = input;
            std::ranges::replace(base64, '-', '+');
            std::ranges::replace(base64, '_', '/');

            while (base64.size() % 4 != 0) {
                base64 += '=';
            }

            static const std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
            std::string result;

            std::vector T(256, -1);
            for (int i = 0; i < 64; i++) T[chars[i]] = i;

            int val = 0, valb = -8;
            for (unsigned char c: base64) {
                if (c == '=') break;
                if (T[c] == -1) continue;
                val = (val << 6) + T[c];
                valb += 6;
                if (valb >= 0) {
                    result.push_back(char((val >> valb) & 0xFF));
                    valb -= 8;
                }
            }
            return result;
        }

        inline std::string parseJwtPayloadRaw(const std::string &jwt) {
            size_t firstDot = jwt.find('.');
            size_t lastDot = jwt.rfind('.');
            if (firstDot == std::string::npos || lastDot == std::string::npos || firstDot >= lastDot) {
                return {};
            }
            std::string payloadStr = jwt.substr(firstDot + 1, lastDot - firstDot - 1);
            return base64UrlDecode(payloadStr);
        }
    }

    class LoginPacket : public PacketType {
    public:
        int32_t protocolVersion = 0;
        std::vector<uint8_t> payload;

        int authenticationType = 0;
        std::vector<std::string> certificateChain;
        std::string authToken;
        std::string clientDataToken;

        std::optional<PlayerIdentity> identity;
        std::optional<MicrosoftAuthToken> microsoftAuth;
        std::optional<ClientData> clientData;

        PacketId getPacketId() const override {
            return PacketId::Login;
        }

        void serialize(binary::cursor &cursor) const override {
        }

        void deserialize(binary::cursor &cursor) override {
            protocolVersion = cursor.readInt32<false>();
            int32_t payloadSize = cursor.readVarInt32();

            std::string authentication = cursor.readStringLE32();
            clientDataToken = cursor.readStringLE32();

            try {
                auto authJson = nlohmann::json::parse(authentication);

                if (authJson.contains("AuthenticationType")) {
                    authenticationType = authJson["AuthenticationType"].get<int>();
                }

                if (authJson.contains("Token")) {
                    authToken = authJson["Token"].get<std::string>();
                }

                if (authJson.contains("Certificate")) {
                    auto certStr = authJson["Certificate"].get<std::string>();
                    auto certJson = nlohmann::json::parse(certStr);

                    if (certJson.contains("chain") && certJson["chain"].is_array()) {
                        for (const auto &jwt: certJson["chain"]) {
                            certificateChain.push_back(jwt.get<std::string>());
                        }
                    }
                }
            } catch (...) {
            }

            for (const auto &jwt: certificateChain) {
                try {
                    std::string decoded = detail::parseJwtPayloadRaw(jwt);
                    if (decoded.empty()) continue;

                    auto jwtPayload = nlohmann::json::parse(decoded);

                    if (jwtPayload.contains("extraData")) {
                        PlayerIdentity id;
                        auto &extra = jwtPayload["extraData"];

                        if (extra.contains("displayName")) {
                            id.displayName = extra["displayName"].get<std::string>();
                        }
                        if (extra.contains("XUID")) {
                            id.xuid = extra["XUID"].get<std::string>();
                        }
                        if (extra.contains("identity")) {
                            id.uuid = extra["identity"].get<std::string>();
                        }
                        if (extra.contains("titleId")) {
                            id.titleId = extra["titleId"].get<std::string>();
                        }
                        if (jwtPayload.contains("identityPublicKey")) {
                            id.identityPublicKey = jwtPayload["identityPublicKey"].get<std::string>();
                        }

                        identity = id;
                    }
                } catch (...) {
                }
            }

            if (!authToken.empty()) {
                try {
                    std::string decoded = detail::parseJwtPayloadRaw(authToken);
                    if (!decoded.empty()) {
                        auto jwtPayload = nlohmann::json::parse(decoded);
                        MicrosoftAuthToken auth;

                        if (jwtPayload.contains("xname")) {
                            auth.xboxGamertag = jwtPayload["xname"].get<std::string>();
                        }
                        if (jwtPayload.contains("xid")) {
                            auth.xboxUid = jwtPayload["xid"].get<std::string>();
                        }
                        if (jwtPayload.contains("mid")) {
                            auth.playFabId = jwtPayload["mid"].get<std::string>();
                        }
                        if (jwtPayload.contains("iss")) {
                            auth.issuer = jwtPayload["iss"].get<std::string>();
                        }
                        if (jwtPayload.contains("aud")) {
                            auth.audience = jwtPayload["aud"].get<std::string>();
                        }

                        microsoftAuth = auth;
                    }
                } catch (...) {
                }
            }

            if (!clientDataToken.empty()) {
                try {
                    std::string decoded = detail::parseJwtPayloadRaw(clientDataToken);
                    if (!decoded.empty()) {
                        auto jwtPayload = nlohmann::json::parse(decoded);
                        ClientData data;

                        if (jwtPayload.contains("SelfSignedId")) {
                            data.selfSignedId = jwtPayload["SelfSignedId"].get<std::string>();
                        }
                        if (jwtPayload.contains("ServerAddress")) {
                            data.serverAddress = jwtPayload["ServerAddress"].get<std::string>();
                        }
                        if (jwtPayload.contains("ClientRandomId")) {
                            data.clientRandomId = jwtPayload["ClientRandomId"].get<int64_t>();
                        }

                        if (jwtPayload.contains("DeviceModel")) {
                            data.deviceModel = jwtPayload["DeviceModel"].get<std::string>();
                        }
                        if (jwtPayload.contains("DeviceOS")) {
                            data.deviceOS = jwtPayload["DeviceOS"].get<int>();
                        }
                        if (jwtPayload.contains("DeviceId")) {
                            data.deviceId = jwtPayload["DeviceId"].get<std::string>();
                        }
                        if (jwtPayload.contains("GameVersion")) {
                            data.gameVersion = jwtPayload["GameVersion"].get<std::string>();
                        }

                        if (jwtPayload.contains("LanguageCode")) {
                            data.languageCode = jwtPayload["LanguageCode"].get<std::string>();
                        }
                        if (jwtPayload.contains("GuiScale")) {
                            data.guiScale = jwtPayload["GuiScale"].get<int>();
                        }
                        if (jwtPayload.contains("UIProfile")) {
                            data.uiProfile = jwtPayload["UIProfile"].get<int>();
                        }

                        if (jwtPayload.contains("DefaultInputMode")) {
                            data.defaultInputMode = jwtPayload["DefaultInputMode"].get<int>();
                        }
                        if (jwtPayload.contains("CurrentInputMode")) {
                            data.currentInputMode = jwtPayload["CurrentInputMode"].get<int>();
                        }

                        if (jwtPayload.contains("SkinId")) {
                            data.skinId = jwtPayload["SkinId"].get<std::string>();
                        }
                        if (jwtPayload.contains("SkinImageWidth")) {
                            data.skinImageWidth = jwtPayload["SkinImageWidth"].get<int>();
                        }
                        if (jwtPayload.contains("SkinImageHeight")) {
                            data.skinImageHeight = jwtPayload["SkinImageHeight"].get<int>();
                        }
                        if (jwtPayload.contains("SkinData")) {
                            data.skinData = jwtPayload["SkinData"].get<std::string>();
                        }
                        if (jwtPayload.contains("SkinResourcePatch")) {
                            data.skinResourcePatch = jwtPayload["SkinResourcePatch"].get<std::string>();
                        }
                        if (jwtPayload.contains("SkinGeometryData")) {
                            data.skinGeometryData = jwtPayload["SkinGeometryData"].get<std::string>();
                        }

                        if (jwtPayload.contains("PlatformUserId")) {
                            data.platformUserId = jwtPayload["PlatformUserId"].get<std::string>();
                        }
                        if (jwtPayload.contains("ThirdPartyName")) {
                            data.thirdPartyName = jwtPayload["ThirdPartyName"].get<std::string>();
                        }
                        if (jwtPayload.contains("PlayFabId")) {
                            data.playFabId = jwtPayload["PlayFabId"].get<std::string>();
                        }

                        if (jwtPayload.contains("TrustedSkin")) {
                            data.trustedSkin = jwtPayload["TrustedSkin"].get<bool>();
                        }
                        if (jwtPayload.contains("PremiumSkin")) {
                            data.premiumSkin = jwtPayload["PremiumSkin"].get<bool>();
                        }
                        if (jwtPayload.contains("PersonaSkin")) {
                            data.personaSkin = jwtPayload["PersonaSkin"].get<bool>();
                        }
                        if (jwtPayload.contains("CapeOnClassicSkin")) {
                            data.capeOnClassicSkin = jwtPayload["CapeOnClassicSkin"].get<bool>();
                        }
                        if (jwtPayload.contains("CapeId")) {
                            data.capeId = jwtPayload["CapeId"].get<std::string>();
                        }

                        if (jwtPayload.contains("CompatibleWithClientSideChunkGen")) {
                            data.compatibleWithClientSideChunkGen = jwtPayload["CompatibleWithClientSideChunkGen"].get<
                                bool>();
                        }
                        if (jwtPayload.contains("IsEditorMode")) {
                            data.isEditorMode = jwtPayload["IsEditorMode"].get<bool>();
                        }

                        clientData = data;
                    }
                } catch (...) {
                }
            }
        }
    };
}
