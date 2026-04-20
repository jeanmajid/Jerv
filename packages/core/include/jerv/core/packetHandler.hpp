#pragma once
#include <array>
#include <cstdint>

namespace jerv::binary {
    class Cursor;
}

namespace jerv::raknet {
    class ServerConnection;
}

namespace jerv::core {
    class Jerver;

    constexpr size_t MAX_PACKET_ID = 345;
    using HandlerFn = void(*)(Jerver&, raknet::ServerConnection&, binary::Cursor&);

    inline std::array<HandlerFn, MAX_PACKET_ID>& getHandlers() {
        static std::array<HandlerFn, MAX_PACKET_ID> handlers{};
        return handlers;
    }

    template<typename PacketType>
    struct PacketRegistrar {
        explicit PacketRegistrar(const HandlerFn handler) {
            const size_t packetId = static_cast<size_t>(PacketType::ID);
            static_assert(packetId > 0 && packetId <= MAX_PACKET_ID, "PacketId invalid");
            getHandlers()[packetId] = handler;
        }
    };
}

