#include "jerv/core/jerver.hpp"

int main() {
    jerv::core::Jerver jerver;
    jerver.bindV4();
    jerver.start();

    return 0;
}