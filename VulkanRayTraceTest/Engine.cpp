#include "Engine.h"

Engine::Engine()
{
}

Engine::Engine(unsigned int width, unsigned int height, const char* WindowName)
{
    window = VulkanWindow(1280, 720, "VulkanEngine");
    engine = VulkanEngine(window.GetWindowPtr());
    renderer = Renderer(engine, window);
}

Engine::~Engine()
{
    renderer.Destroy(engine);
    engine.Destroy();
    window.Destroy();
}

void Engine::MainLoop()
{
    while (!glfwWindowShouldClose(window.GetWindowPtr())) 
    {
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