#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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
#include "RayTraceRenderer.h"
#include "VulkanWindow.h"
#include "VulkanEngine.h"
#include "ForwardRenderPass.h"
#include "Renderer.h"

class HelloTriangleApplication {
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

int main() {
    HelloTriangleApplication app;

    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

