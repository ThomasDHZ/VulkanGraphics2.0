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
#include "InterfaceRenderPass.h"
#include "RayTraceRenderPass.h"
#include "VulkanWindow.h"
#include "VulkanEngine.h"
#include "ForwardRenderPass.h"
#include "Renderer.h"

class Engine {
private:
    VulkanWindow window;
    VulkanEngine engine;
    Renderer renderer;

public:
    Engine();
    Engine(unsigned int width, unsigned int height, const char* WindowName);
    ~Engine();

    void MainLoop();
};