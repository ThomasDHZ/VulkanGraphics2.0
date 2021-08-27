#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


#include "VulkanWindow.h"

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
#include "VulkanEngine.h"
#include "Texture2D.h"
#include "Mesh.h"
#include "InterfaceRenderPass.h"
#include "BlinnPhongPipeline.h"
#include "RenderedDepthTexture.h"
#include "RendererManager.h"
#include "BlinnPhongRasterPass.h"
#include "CameraManager.h"
#include "InputManager.h"
#include "TextureManager.h"
#include "MaterialManager.h"
#include "MeshManager.h"
#include "LightManager.h"
#include "GUIManager.h"
#include "ObjectManager.h"
#include "AssetManager.h"

class HelloTriangleApplication {
public:
    void run() {
        initVulkan();
        mainLoop();

        AssetManagerPtr::GetAssetPtr()->Delete();
        renderer.Destroy(EnginePtr::GetEnginePtr());
        EnginePtr::GetEnginePtr()->Destroy();
        WindowPtr::GetWindowPtr()->Destroy();
    }

private:
    RendererManager renderer;

    void initVulkan() {

        WindowPtr::SetUpPtr(1280, 720, "VulkanEngine");
        EnginePtr::SetUpPtr(WindowPtr::GetWindowPtr());
        CameraManagerPtr::SetUpPtr(EnginePtr::GetEnginePtr());
        InputManagerPtr::SetUpPtr(WindowPtr::GetWindowPtr(), CameraManagerPtr::GetCameraManagerPtr());
        TextureManagerPtr::SetUpPtr(EnginePtr::GetEnginePtr());
        MaterialManagerPtr::SetUpPtr(EnginePtr::GetEnginePtr());
        MeshManagerPtr::SetUpPtr(EnginePtr::GetEnginePtr(), InputManagerPtr::GetInputManagerPtr(), MaterialManagerPtr::GetMaterialManagerPtr());
        LightManagerPtr::SetUpPtr(EnginePtr::GetEnginePtr(), CameraManagerPtr::GetCameraManagerPtr());
        GuiManagerPtr::SetUpPtr(EnginePtr::GetEnginePtr());
        ObjManagerPtr::SetUpPtr(EnginePtr::GetEnginePtr());
        AssetManagerPtr::SetUpPtr(EnginePtr::GetEnginePtr());

        renderer = RendererManager(EnginePtr::GetEnginePtr(), WindowPtr::GetWindowPtr());
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(WindowPtr::GetWindowPtr()->GetWindowPtr())) {
            glfwPollEvents();
            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            {
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            }
            ImGui::Render();
            renderer.Draw(EnginePtr::GetEnginePtr(), WindowPtr::GetWindowPtr());
        }

        vkDeviceWaitIdle(EnginePtr::GetEnginePtr()->Device);
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