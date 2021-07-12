#include "Game.h"
#include <exception>
#include <iostream>

int main() {
    Game game = Game();

    try {
        game.MainLoop();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

