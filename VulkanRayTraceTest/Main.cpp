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
#include "FrameBufferRenderPass.h"
#include "Renderer.h"

const uint32_t WIDTH = 1920;
const uint32_t HEIGHT = 1080;

 std::vector<Vertex> vertices =
{
	{{-0.5,-0.5,-0.5}, {0,-1,0}, {0,1}},
	{{0.5,-0.5,-0.5}, {0,-1,0}, {1,1}},
	{{0.5,-0.5,0.5}, {0,-1,0}, {1,0}},
	{{-0.5,-0.5,0.5}, {0,-1,0}, {0,0}},
	{{-0.5,0.5,-0.5}, {0,0,-1}, {0,0}},
	{{0.5,0.5,-0.5}, {0,0,-1}, {1,0}},
	{{0.5,-0.5,-0.5}, {0,0,-1}, {1,1}},
	{{-0.5,-0.5,-0.5}, {0,0,-1}, {0,1}},
	{{0.5,0.5,-0.5}, {1,0,0}, {0,0}},
	{{0.5,0.5,0.5}, {1,0,0}, {1,0}},
	{{0.5,-0.5,0.5}, {1,0,0}, {1,1}},
	{{0.5,-0.5,-0.5}, {1,0,0}, {0,1}},
	{{0.5,0.5,0.5}, {0,0,1}, {1,0}},
	{{-0.5,0.5,0.5}, {0,0,1}, {0,0}},
	{{-0.5,-0.5,0.5}, {0,0,1}, {0,1}},
	{{0.5,-0.5,0.5}, {0,0,1}, {1,1}},
	{{-0.5,0.5,0.5}, {-1,0,0}, {1,0}},
	{{-0.5,0.5,-0.5}, {-1,0,0}, {0,0}},
	{{-0.5,-0.5,-0.5}, {-1,0,0}, {0,1}},
	{{-0.5,-0.5,0.5}, {-1,0,0}, {1,1}},
	{{-0.5,0.5,-0.5}, {0,1,0}, {0,1}},
	{{-0.5,0.5,0.5}, {0,1,0}, {0,0}},
	{{0.5,0.5,0.5}, {0,1,0}, {1,0}},
	{{0.5,0.5,-0.5}, {0,1,0}, {1,1}},
};

 std::vector<uint32_t> indices = {
	0,1,2, 0,2,3,
	4,5,6, 4,6,7,
	8,9,10, 8,10,11,
	12,13,14, 12,14,15,
	16,17,18, 16,18,19,
	20,21,22, 20,22,23,
};

class HelloTriangleApplication {
public:
    void run() {
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    bool RayTraceSwitch = false;

    VulkanWindow window;
    VulkanEngine engine;
    Keyboard keyboard;
    Mouse mouse;
    TextureManager textureManager;

    Renderer renderer;

    std::vector<RayTraceModel> ModelList;
    VulkanBuffer MaterialBuffer;
    std::shared_ptr<PerspectiveCamera> camera;
    std::shared_ptr<SceneDataStruct> SceneData;


    std::vector<VkCommandBuffer> commandBuffers;




    size_t currentFrame = 0;

    bool framebufferResized = false;


    void initVulkan() {
        window = VulkanWindow(WIDTH, HEIGHT, "VulkanEngine");
        engine = VulkanEngine(window.GetWindowPtr());

        textureManager = TextureManager();


        ModelList.emplace_back(RayTraceModel(engine, textureManager, "C:/Users/dotha/source/repos/VulkanGraphics/Models/Sponza/Sponza.obj"));
        SceneData = std::make_shared<SceneDataStruct>(SceneDataStruct(engine));


        renderer = Renderer(engine, window, textureManager, ModelList);
    
        renderer.SetUpDescriptorSets(engine, textureManager, ModelList, MaterialBuffer, SceneData);
        createCommandBuffers();

        camera = std::make_shared<PerspectiveCamera>(glm::vec2(engine.SwapChain.SwapChainResolution.width, engine.SwapChain.SwapChainResolution.height), glm::vec3(0.0f, 0.0f, 5.0f));


        SceneData->SceneData.dlight.direction = glm::vec4(28.572f, 1000.0f, 771.429f, 0.0f);
        SceneData->SceneData.dlight.ambient = glm::vec4(0.2f);
        SceneData->SceneData.dlight.diffuse = glm::vec4(0.5f);
        SceneData->SceneData.dlight.specular = glm::vec4(1.0f);

        SceneData->SceneData.plight.position = glm::vec4(0.0f);
        SceneData->SceneData.plight.ambient = glm::vec4(0.2f);
        SceneData->SceneData.plight.diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 0.0f);
        SceneData->SceneData.plight.specular = glm::vec4(1.0f);

    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window.GetWindowPtr())) {
            glfwPollEvents();

            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            {
              ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
              ImGui::Checkbox("RayTraceSwitch", &RayTraceSwitch);
              ImGui::SliderFloat3("Pos", &SceneData->SceneData.dlight.direction.x, -1000.0f, 1000.0f);
              ImGui::SliderFloat3("Ambient", &SceneData->SceneData.dlight.ambient.x, 0.0f, 1.0f);
              ImGui::SliderFloat3("Diffuse", &SceneData->SceneData.dlight.diffuse.x, 0.0f, 1.0f);
              ImGui::SliderFloat3("Speculare", &SceneData->SceneData.dlight.specular.x, 0.0f, 1.0f);

              ImGui::SliderFloat3("Pos2", &SceneData->SceneData.plight.position.x, -10.0f, 10.0f);
              ImGui::SliderFloat3("Ambient2", &SceneData->SceneData.plight.ambient.x, 0.0f, 1.0f);
              ImGui::SliderFloat3("Diffuse2", &SceneData->SceneData.plight.diffuse.x, 0.0f, 1.0f);
              ImGui::SliderFloat3("Speculare2", &SceneData->SceneData.plight.specular.x, 0.0f, 1.0f);
              ImGui::SliderFloat("constant", &SceneData->SceneData.plight.constant, 0.0f, 100.0f);
              ImGui::SliderFloat("linear", &SceneData->SceneData.plight.linear, 0.0f, 100.0f);
              ImGui::SliderFloat("quadratic", &SceneData->SceneData.plight.quadratic, 0.0f, 100.0f);
            }
            ImGui::Render();

            drawFrame();
        }

        vkDeviceWaitIdle(engine.Device);
    }

    void cleanup() 
    {
        for (auto model : ModelList)
        {
            model.Destory(engine.Device);
        }
        textureManager.Destory(engine);
        renderer.interfaceRenderPass.Destroy(engine.Device);
        renderer.RenderPass.Destroy(engine);
        renderer.RayRenderer.Destory(engine);

        vkDestroyDescriptorPool(engine.Device, renderer.descriptorPool, nullptr);
        vkDestroyDescriptorSetLayout(engine.Device, renderer.descriptorSetLayout, nullptr);

        engine.Destroy();
        window.Destroy();
    }

    void recreateSwapChain() {
        int width = 0, height = 0;
        glfwGetFramebufferSize(window.GetWindowPtr(), &width, &height);
        while (width == 0 || height == 0) {
            glfwGetFramebufferSize(window.GetWindowPtr(), &width, &height);
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(engine.Device);

        for (auto imageView : engine.SwapChain.GetSwapChainImageViews()) {
            vkDestroyImageView(engine.Device, imageView, nullptr);
        }

        vkDestroySwapchainKHR(engine.Device, engine.SwapChain.GetSwapChain(), nullptr);
        vkDestroyDescriptorPool(engine.Device, renderer.descriptorPool, nullptr);

        renderer.SetUpDescriptorPool(engine);
        renderer.SetUpDescriptorSets(engine, textureManager, ModelList, MaterialBuffer, SceneData);

        engine.SwapChain.UpdateSwapChain(window.GetWindowPtr(), engine.Device, engine.PhysicalDevice, engine.Surface);
        renderer.RenderPass.UpdateSwapChain(engine, renderer.descriptorSetLayout);
        renderer.interfaceRenderPass.UpdateSwapChain(engine.Device, engine.SwapChain.SwapChainImageViews, engine.SwapChain.SwapChainResolution);
        renderer.RayRenderer.Resize(engine, engine.SwapChain.SwapChainImages.size(), engine.SwapChain.SwapChainImages, engine.SwapChain.SwapChainResolution.width, engine.SwapChain.SwapChainResolution.height);

        createCommandBuffers();
    }

    void createCommandBuffers() {
        commandBuffers.resize(renderer.RenderPass.SwapChainFramebuffers.size());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = engine.CommandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

        if (vkAllocateCommandBuffers(engine.Device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }

        for (size_t i = 0; i < commandBuffers.size(); i++) {
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
                throw std::runtime_error("failed to begin recording command buffer!");
            }

            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = renderer.RenderPass.RenderPass;
            renderPassInfo.framebuffer = renderer.RenderPass.SwapChainFramebuffers[i];
            renderPassInfo.renderArea.offset = { 0, 0 };
            renderPassInfo.renderArea.extent = engine.SwapChain.SwapChainResolution;

            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
            clearValues[1].depthStencil = { 1.0f, 0 };

            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
            vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, renderer.RenderPass.forwardRendereringPipeline->ShaderPipeline);
            vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, renderer.RenderPass.forwardRendereringPipeline->ShaderPipelineLayout, 0, 1, &renderer.descriptorSets[i], 0, nullptr);

            for (auto model : ModelList)
            {
                model.Draw(commandBuffers[i], renderer.RenderPass.forwardRendereringPipeline);
            }

            vkCmdEndRenderPass(commandBuffers[i]);
           // frameBufferRenderPass.Draw(engine, commandBuffers[i], i);


            if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to record command buffer!");
            }
        }
    }

    void updateUniformBuffer(uint32_t currentImage) {

        keyboard.Update(window.GetWindowPtr(), camera);
        mouse.Update(window.GetWindowPtr(), camera);
        camera->Update(engine.SwapChain.SwapChainResolution.width, engine.SwapChain.SwapChainResolution.height);

        static auto startTime = std::chrono::high_resolution_clock::now();

        auto  currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        SceneData->SceneData.model = glm::mat4(1.0f);
        SceneData->SceneData.viewInverse = glm::inverse(camera->GetViewMatrix());
        SceneData->SceneData.projInverse = glm::inverse(camera->GetProjectionMatrix());
        SceneData->SceneData.projInverse[1][1] *= -1;
        SceneData->SceneData.view = camera->GetViewMatrix(); 
        SceneData->SceneData.proj = camera->GetProjectionMatrix();
        SceneData->SceneData.proj[1][1] *= -1;
        SceneData->SceneData.viewPos = glm::vec4(camera->GetPosition(), 0.0f);
        SceneData->SceneData.vertexSize = sizeof(Vertex);
        SceneData->Update(engine);
    }

    void drawFrame() {
        vkWaitForFences(engine.Device, 1, &engine.inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(engine.Device, engine.SwapChain.GetSwapChain(), UINT64_MAX, engine.vulkanSemaphores[currentFrame].ImageAcquiredSemaphore, VK_NULL_HANDLE, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        renderer.interfaceRenderPass.Draw(engine.Device, imageIndex, engine.SwapChain.SwapChainResolution);
        renderer.RayRenderer.updateUniformBuffers(engine, window.GetWindowPtr(), SceneData->SceneData, camera);
        updateUniformBuffer(imageIndex);

        if (engine.imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
            vkWaitForFences(engine.Device, 1, &engine.imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
        }
        engine.imagesInFlight[imageIndex] = engine.inFlightFences[currentFrame];

        VkSemaphore waitSemaphores[] = { engine.vulkanSemaphores[currentFrame].ImageAcquiredSemaphore };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

        std::vector<VkCommandBuffer> CommandBufferSubmitList;
        if (!RayTraceSwitch)
        {
            CommandBufferSubmitList.emplace_back(commandBuffers[imageIndex]);
            CommandBufferSubmitList.emplace_back(renderer.interfaceRenderPass.ImGuiCommandBuffers[imageIndex]);
        }
        else
        {
           CommandBufferSubmitList.emplace_back(renderer.RayRenderer.drawCmdBuffers[imageIndex]);
            CommandBufferSubmitList.emplace_back(renderer.interfaceRenderPass.ImGuiCommandBuffers[imageIndex]);
        }

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = static_cast<uint32_t>(CommandBufferSubmitList.size());
        submitInfo.pCommandBuffers = CommandBufferSubmitList.data();

        VkSemaphore signalSemaphores[] = { engine.vulkanSemaphores[currentFrame].RenderCompleteSemaphore };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;


        vkResetFences(engine.Device, 1, &engine.inFlightFences[currentFrame]);

        if (vkQueueSubmit(engine.GraphicsQueue, 1, &submitInfo, engine.inFlightFences[currentFrame]) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = { engine.SwapChain.GetSwapChain() };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;

        presentInfo.pImageIndices = &imageIndex;

        result = vkQueuePresentKHR(engine.PresentQueue, &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
            framebufferResized = false;
            recreateSwapChain();
        }
        else if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
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

