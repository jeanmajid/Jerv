#include <fstream>
#include <jerv/core/jerver.hpp>
#include <jerv/core/events.hpp>
#include <jerv/protocol/protocol.hpp>
#include <jerv/common/logger.hpp>
#include <jerv/binary/nbt.hpp>
#include <iostream>

std::vector<uint8_t> read_file(const std::string& path)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file)
        throw std::runtime_error("Failed to open file");

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(size);

    if (!file.read(reinterpret_cast<char*>(buffer.data()), size))
        throw std::runtime_error("Failed to read file");

    return buffer;
}

int main() {
    try {
        // jerv::Jerver jerver;
        //
        // // jerver.onPacketType(jerv::protocol::PacketId::Animate, [](jerv::core::PacketEvent& event) {
        // //     LOG_INFO("Swing");
        // // });
        //
        // jerver.onPlayerJoin = [](jerv::core::ConnectionEvent &event) {
        //     JERV_LOG_INFO("player joined");
        // };
        //
        // jerver.onPlayerLeave = [](jerv::core::ConnectionEvent &event) {
        //     JERV_LOG_INFO("player left");
        // };
        //
        // jerver.server().bindV4();
        // jerver.start();

        auto buffer = read_file("C:/Users/jeanh/AppData/Roaming/Minecraft Bedrock/Users/17010935870061832014/games/com.mojang/minecraftWorlds/ma6/level.dat");
        jerv::binary::NBT nbt(buffer);
        while (!nbt.isEnd()) {
            jerv::binary::NBTData data = nbt.next();
            data.print();
        }
    } catch (const std::exception &e) {
        JERV_LOG_ERROR("fatal error: {}", e.what());
    }

    return 0;
}
