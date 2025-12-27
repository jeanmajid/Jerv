#pragma once

#include <string>
#include <cstdint>
#include <span>
#include <functional>

namespace jerv::raknet {
    struct AddressInfo {
        std::string address;
        uint16_t port;
        std::string family;

        bool operator==(const AddressInfo &other) const {
            return address == other.address && port == other.port;
        }

        std::string toString() const {
            return address + "#" + std::to_string(port);
        }
    };

    struct FrameDescriptor {
        std::span<uint8_t> body;

        struct FragmentInfo {
            uint16_t id;
            uint32_t index;
            uint32_t length;
        };

        FragmentInfo *fragment = nullptr;
        uint8_t orderChannel = 0;
        uint32_t orderIndex = 0;
        uint32_t reliableIndex = 0;
        uint32_t sequenceIndex = 0;
        size_t offset = 0;
    };

    struct SocketSource {
        std::function<void(std::function < void(std::span<uint8_t>, const AddressInfo &) >)> onDataCallback;
        std::function<void(std::span<const uint8_t>, const AddressInfo &)> send;
    };

    class Connection {
    public:
        virtual ~Connection() = default;
        virtual void send(std::span<const uint8_t> data, uint8_t reliability) = 0;
        virtual void disconnect() = 0;
        virtual const std::string &id() const = 0;
    };

    class ServerListener {
    public:
        virtual ~ServerListener() = default;
        virtual void addListenerSource(SocketSource &source) = 0;
    };
}