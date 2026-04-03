#include "jerv/raknet/RaknetServer.hpp"

int main() {
    // jerv::Jerver jerver;
    //
    // jerver.server().bindV4();
    // jerver.start();

    jerv::raknet::RaknetServer server;
    server.bindV4();
    server.start();


    return 0;
}
