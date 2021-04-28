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
public:
    void run() {
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    VulkanWindow window;
    VulkanEngine engine;
    Renderer renderer;

    void initVulkan()
    {
        window = VulkanWindow(1280, 720, "VulkanEngine");
        engine = VulkanEngine(window.GetWindowPtr());
        renderer = Renderer(engine, window);
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window.GetWindowPtr())) {
            glfwPollEvents();

            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            {
                renderer.GUIUpdate(engine);
            }
            ImGui::Render();

            renderer.Draw(engine, window);
        }

        vkDeviceWaitIdle(engine.Device);
    }

    void cleanup()
    {
        renderer.Destroy(engine);
        engine.Destroy();
        window.Destroy();
    }
};