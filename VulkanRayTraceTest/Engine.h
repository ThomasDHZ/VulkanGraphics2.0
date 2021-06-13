#pragma once
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <array>
#include <optional>
#include <set>
#include "VulkanWindow.h"
#include "VulkanEngine.h"
#include "Renderer.h"

class Engine {
private:
    VulkanEngine engine;
    Renderer renderer;
    std::shared_ptr<VulkanWindow> window;
    std::shared_ptr<AssetManager> assetManager;

public:
    Engine();
    Engine(unsigned int width, unsigned int height, const char* WindowName);
    ~Engine();

    void MainLoop();
};