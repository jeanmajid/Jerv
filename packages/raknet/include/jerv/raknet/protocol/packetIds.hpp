#pragma once

namespace jerv::raknet {
    enum class RaknetPacketId {
        ConnectedPing = 0x00,
        ConnectedPong = 0x00,
        UnconnectPing = 0x01,
        UnconnectPong = 0x1c,
        OpenConnectionRequest1 = 0x05,
        OpenConnectionReply1 = 0x06,
        OpenConnectionRequest2 = 0x07,
        OpenConnectionReply2 = 0x08,
        ConnectionRequest = 0x09,
        ConnectionRequestAccepted = 0x10,
        NewIncomingConnection = 0x13,
        AckDatagram = 0xC0,
        NackDatagram = 0xA0,
        Disconnect = 0x15,
        GameData = 0xFE
    };
}