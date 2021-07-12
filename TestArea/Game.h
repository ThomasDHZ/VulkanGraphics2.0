#pragma once
#include "../VulkanRayTraceTest/Engine.h"

class Game
{

private:
    Engine engine;

public:
    Game();
    ~Game();

    void MainLoop();
};
