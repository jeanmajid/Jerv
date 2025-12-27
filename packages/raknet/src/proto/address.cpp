#include <jerv/raknet/proto.hpp>
#include <cstring>
#include <sstream>

namespace jerv::raknet::proto {
    void writeAddress(uint8_t *data, size_t &offset, const AddressInfo &address) {
        if (address.family == "IPv4") {
            data[offset++] = 4;

            std::istringstream iss(address.address);
            std::string octet;
            uint8_t octets[4];
            int i = 0;
            while (std::getline(iss, octet, '.') && i < 4) {
                octets[i++] = ~static_cast<uint8_t>(std::stoi(octet));
            }
            std::memcpy(data + offset, octets, 4);
            offset += 4;

            data[offset++] = static_cast<uint8_t>(address.port >> 8);
            data[offset++] = static_cast<uint8_t>(address.port);
        } else {
            data[offset++] = 6;

            data[offset++] = static_cast<uint8_t>(address.port);
            data[offset++] = static_cast<uint8_t>(address.port >> 8);

            std::memset(data + offset, 0, 4);
            offset += 4;

            std::memset(data + offset, 0, 16);
            offset += 16;

            std::memset(data + offset, 0, 4);
            offset += 4;
        }
    }

    AddressInfo readAddress(const uint8_t *data, size_t &offset) {
        AddressInfo address;
        uint8_t type = data[offset++];

        if (type == 4) {
            address.family = "IPv4";

            std::ostringstream oss;
            for (int i = 0; i < 4; i++) {
                if (i > 0) oss << ".";
                oss << ~data[offset++];
            }
            address.address = oss.str();

            address.port = (static_cast<uint16_t>(data[offset]) << 8) | data[offset + 1];
            offset += 2;
        } else {
            address.family = "IPv6";

            address.port = data[offset] | (static_cast<uint16_t>(data[offset + 1]) << 8);
            offset += 2;
            offset += 4;

            address.address = "::";
            offset += 16;
            offset += 4;
        }

        return address;
    }
}