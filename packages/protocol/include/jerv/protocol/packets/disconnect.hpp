#pragma once

#include <jerv/protocol/packet.hpp>
#include <string>

namespace jerv::protocol {
    enum DisconnectReason : uint8_t {
        Unknown = 0,
        CantConnectNoInternet = 1,
        NoPermissions = 2,
        UnrecoverableError = 3,
        ThirdPartyBlocked = 4,
        ThirdPartyNoInternet = 5,
        ThirdPartyBadIP = 6,
        ThirdPartyNoServerOrServerLocked = 7,
        VersionMismatch = 8,
        SkinIssue = 9,
        InviteSessionNotFound = 10,
        EduLevelSettingsNotAllowed = 11,
        LocalServerNotFound = 12,
        LegacyDisconnect = 13,
        UserLeaveGameAttempted = 14,
        PlatformLockedSkinError = 15,
        RealmsWorldUnassigned = 16,
        RealmsServerCantConnect = 17,
        RealmsServerHidden = 18,
        RealmsServerDisabledBeta = 19,
        RealmsServerDisabled = 20,
        CrossPlatformDisabled = 21,
        CantConnect = 22,
        SessionNotFound = 23,
        ClientSettingsNotCompatible = 24,
        ServerFull = 25,
        InvalidPlatformSkin = 26,
        EditionVersionMismatch = 27,
        EditionMismatch = 28,
        LevelNewerThanExe = 29,
        NoFailOccurred = 30,
        BannedSkin = 31,
        Timeout = 32,
        ServerNotFound = 33,
        OutdatedServer = 34,
        OutdatedClient = 35,
        NoPremiumPlatform = 36,
        MultiplayerDisabled = 37,
        NoWifi = 38,
        WorldCorruption = 39,
        NoReason = 40,
        Disconnected = 41,
        InvalidPlayer = 42,
        LoggedInOtherLocation = 43,
        ServerIdConflict = 44,
        NotAllowed = 45,
        NotAuthenticated = 46,
        InvalidTenant = 47,
        UnknownPacket = 48,
        UnexpectedPacket = 49,
        InvalidCommandRequestPacket = 50,
        HostSuspended = 51,
        LoginPacketNoRequest = 52,
        LoginPacketNoCert = 53,
        MissingClient = 54,
        Kicked = 55,
        KickedForExploit = 56,
        KickedForIdle = 57,
        ResourcePackProblem = 58,
        IncompatiblePack = 59,
        OutOfStorage = 60,
        InvalidLevel = 61,
        DisconnectPacketDeprecated = 62,
        BlockMismatch = 63,
        InvalidHeights = 64,
        InvalidWidths = 65,
        ConnectionLost = 66,
        ZombieConnection = 67,
        Shutdown = 68,
        ReasonNotSet = 69,
        LoadingStateTimeout = 70,
        ResourcePackLoadingFailed = 71,
        SearchingForSessionLoadingScreenFailed = 72,
        ConnProtocolVersion = 73,
        SubsystemStatusError = 74,
        EmptyAuthFromDiscovery = 75,
        EmptyUrlFromDiscovery = 76,
        ExpiredAuthFromDiscovery = 77,
        UnknownSignalServiceSignInFailure = 78,
        XBLJoinLobbyFailure = 79,
        UnspecifiedClientInstanceDisconnection = 80,
        NetherNetNoInternet = 81,
        NetherNetNoSignInSignal = 82,
        NetherNetLoginToNetherNetFailed = 83,
        NetherNetNotLoggedIn = 84,
        NetherNetSessionNotFound = 85,
        NetherNetCreatePeerConnection = 86,
        NetherNetICE = 87,
        NetherNetConnectRequest = 88,
        NetherNetConnectResponse = 89,
        NetherNetNegotiationTimeout = 90,
        NetherNetInactivityTimeout = 91,
        StaleConnectionBeingReplaced = 92,
        RealmsSessionNotFound = 93,
        BadPacket = 94
    };

    class DisconnectPacket : public PacketType {
    public:
        DisconnectReason reason = DisconnectReason::Unknown;
        bool hideDisconnectScreen = false;
        std::string message;

        PacketId getPacketId() const override {
            return PacketId::Disconnect;
        }

        void serialize(binary::cursor &cursor) const override {
            cursor.writeVarInt(static_cast<int32_t>(reason));
            cursor.writeUint8(hideDisconnectScreen ? 1 : 0);
            if (!hideDisconnectScreen) {
                cursor.writeString(message);
            }
        }

        void deserialize(binary::cursor &cursor) override {
            reason = static_cast<DisconnectReason>(cursor.readVarInt());
            hideDisconnectScreen = cursor.readUint8() != 0;
            if (!hideDisconnectScreen) {
                message = cursor.readString();
            }
        }

        static DisconnectPacket from(const DisconnectReason reason) {
            DisconnectPacket packet;
            packet.reason = reason;
            return packet;
        }
    };
}
