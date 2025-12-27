#pragma once

#include <cstdint>

namespace jerv::raknet {
    enum class Reliability : uint8_t {
        Unreliable = 0,
        UnreliableSequenced = 1,
        Reliable = 2,
        ReliableOrdered = 3,
        ReliableSequenced = 4
    };

    enum class UnconnectedPacketId : uint8_t {
        UnconnectedPing = 0x01,
        UnconnectedPong = 0x1C,
        OpenConnectionRequestOne = 0x05,
        OpenConnectionReplyOne = 0x06,
        OpenConnectionRequestTwo = 0x07,
        OpenConnectionReplyTwo = 0x08,
        AckDatagram = 0xC0,
        NackDatagram = 0xA0,
        ValidDatagram = 0x80
    };

    enum class ConnectedPacketId : uint8_t {
        ConnectedPing = 0x00,
        ConnectedPong = 0x03,
        ConnectionRequest = 0x09,
        ConnectionRequestAccepted = 0x10,
        NewIncomingConnection = 0x13,
        Disconnect = 0x15,
        GameData = 0xFE
    };
}