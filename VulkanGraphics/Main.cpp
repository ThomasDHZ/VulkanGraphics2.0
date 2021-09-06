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
#include "MainRenderPass.h"
#include "Renderer.h"
#include "RendererManager.h"

const uint32_t WIDTH = 1280;
const uint32_t HEIGHT = 720;

class HelloTriangleApplication {
public:
    void run() {
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    std::shared_ptr<VulkanWindow> window;
    VulkanEngine engine;
    //RendererManager renderer;
    Renderer renderer;

    std::vector<Model> ModelList;

    void initVulkan() 
    {
        window = std::make_shared<VulkanWindow>(VulkanWindow(WIDTH, HEIGHT, "VulkanEngine"));
        engine = VulkanEngine(window);

        WindowPtr::SetUpPtr(window);
        EnginePtr::SetUpPtr(std::make_shared<VulkanEngine>(engine));

       // renderer = RendererManager(EnginePtr::GetEnginePtr(), WindowPtr::GetWindowPtr());
        renderer = Renderer(engine, window);

        //renderer.AddModel(engine, window, textureManager, "");
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window->GetWindowPtr())) {
            glfwPollEvents();

            //ImGui_ImplVulkan_NewFrame();
            //ImGui_ImplGlfw_NewFrame();
            //ImGui::NewFrame();
            //{
            //    renderer.GUIUpdate(engine);
            //   // renderer.GUIUpdate(EnginePtr::GetEnginePtr());
            //}
            //ImGui::Render();

            renderer.Draw(engine, window);
            //renderer.Draw(EnginePtr::GetEnginePtr(), WindowPtr::GetWindowPtr());
        }

        vkDeviceWaitIdle(engine.Device);
    }

    void cleanup() 
    {
        renderer.Destroy(engine);
       // renderer.Destroy(EnginePtr::GetEnginePtr());
        engine.Destroy();
        window->Destroy();
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

