#include "Game.h"
#include <iostream>

int main() {
    Game app;

    try
    {
        app.MainLoop();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
