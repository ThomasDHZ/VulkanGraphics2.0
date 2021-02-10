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

const uint32_t WIDTH = 1920;
const uint32_t HEIGHT = 1080;

std::vector<Vertex> vertices = {
    {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
    {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},

    {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
    {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
};

std::vector<uint32_t> indices = {
    0, 1, 2, 2, 3, 0,
    4, 5, 6, 6, 7, 4
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
    RayTraceRenderer RayRenderer;
    InterfaceRenderPass interfaceRenderPass;
    MainRenderPass RenderPass;

    TextureManager textureManager;

    std::vector<RayTraceModel> mesh;
    SceneDataBufferData ubo{};

    VkDescriptorPool descriptorPool;
    VkDescriptorSetLayout descriptorSetLayout;
    std::vector<VkDescriptorSet> descriptorSets;

    std::vector<VkCommandBuffer> commandBuffers;

    size_t currentFrame = 0;

    bool framebufferResized = false;


    void initVulkan() {
        window = VulkanWindow(WIDTH, HEIGHT, "VulkanEngine");
        engine = VulkanEngine(window.GetWindowPtr());

        createDescriptorSetLayout();
        RenderPass = MainRenderPass(engine, descriptorSetLayout);

        interfaceRenderPass = InterfaceRenderPass(engine.Device, engine.Instance, engine.PhysicalDevice, engine.GraphicsQueue, window.GetWindowPtr(), engine.SwapChain.SwapChainImageViews, engine.SwapChain.SwapChainResolution);
        textureManager = TextureManager();
        auto a = textureManager.LoadTexture(engine, "C:/Users/dotha/source/repos/VulkanGraphics/texture/texture.jpg", VK_FORMAT_R8G8B8A8_SRGB);

        uint32_t VertexCount = vertices.size();
        uint32_t IndexCount = indices.size();
        mesh.emplace_back(RayTraceModel(engine, vertices, indices));

        createDescriptorPool();
        createDescriptorSets();
        createCommandBuffers();


        ubo.dlight.direction = glm::vec4(28.572f, 1000.0f, 771.429f, 0.0f);
        ubo.dlight.ambient = glm::vec4(0.2f);
        ubo.dlight.diffuse = glm::vec4(0.5f);
        ubo.dlight.specular = glm::vec4(1.0f);

        ubo.plight.position = glm::vec4(0.0f);
        ubo.plight.ambient = glm::vec4(0.2f);
        ubo.plight.diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 0.0f);
        ubo.plight.specular = glm::vec4(1.0f);

        RayRenderer = RayTraceRenderer(engine, textureManager, mesh);
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
              ImGui::SliderFloat3("Pos", &ubo.dlight.direction.x, -1000.0f, 1000.0f);
              ImGui::SliderFloat3("Ambient", &ubo.dlight.ambient.x, 0.0f, 1.0f);
              ImGui::SliderFloat3("Diffuse", &ubo.dlight.diffuse.x, 0.0f, 1.0f);
              ImGui::SliderFloat3("Speculare", &ubo.dlight.specular.x, 0.0f, 1.0f);

              ImGui::SliderFloat3("Pos2", &ubo.plight.position.x, -10.0f, 10.0f);
              ImGui::SliderFloat3("Ambient2", &ubo.plight.ambient.x, 0.0f, 1.0f);
              ImGui::SliderFloat3("Diffuse2", &ubo.plight.diffuse.x, 0.0f, 1.0f);
              ImGui::SliderFloat3("Speculare2", &ubo.plight.specular.x, 0.0f, 1.0f);
              ImGui::SliderFloat("constant", &ubo.plight.constant, 0.0f, 100.0f);
              ImGui::SliderFloat("linear", &ubo.plight.linear, 0.0f, 100.0f);
              ImGui::SliderFloat("quadratic", &ubo.plight.quadratic, 0.0f, 100.0f);
            }
            ImGui::Render();

            drawFrame();
        }

        vkDeviceWaitIdle(engine.Device);
    }

    void cleanup() 
    {
        mesh[0].Destory(engine.Device);
        textureManager.Destory(engine);
        interfaceRenderPass.Destroy(engine.Device);
        RenderPass.Destroy(engine);
        RayRenderer.Destory(engine);

        vkDestroyDescriptorPool(engine.Device, descriptorPool, nullptr);
        vkDestroyDescriptorSetLayout(engine.Device, descriptorSetLayout, nullptr);

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
        vkDestroyDescriptorPool(engine.Device, descriptorPool, nullptr);

        createDescriptorPool();
        createDescriptorSets();

        engine.SwapChain.UpdateSwapChain(window.GetWindowPtr(), engine.Device, engine.PhysicalDevice, engine.Surface);
        RenderPass.UpdateSwapChain(engine, descriptorSetLayout);
        interfaceRenderPass.UpdateSwapChain(engine.Device, engine.SwapChain.SwapChainImageViews, engine.SwapChain.SwapChainResolution);
        RayRenderer.Resize(engine, engine.SwapChain.SwapChainImages.size(), engine.SwapChain.SwapChainImages, engine.SwapChain.SwapChainResolution.width, engine.SwapChain.SwapChainResolution.height);

        createCommandBuffers();
    }


    void createDescriptorSetLayout() {
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.pImmutableSamplers = nullptr;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutBinding samplerLayoutBinding{};
        samplerLayoutBinding.binding = 6;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.pImmutableSamplers = nullptr;
        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();

        if (vkCreateDescriptorSetLayout(engine.Device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    void createDescriptorPool() {
        std::array<VkDescriptorPoolSize, 2> poolSizes{};
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(engine.SwapChain.SwapChainImages.size());
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(engine.SwapChain.SwapChainImages.size());
        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(engine.SwapChain.SwapChainImages.size());

        if (vkCreateDescriptorPool(engine.Device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    void createDescriptorSets() {
        std::vector<VkDescriptorSetLayout> layouts(engine.SwapChain.SwapChainImages.size(), descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(engine.SwapChain.SwapChainImages.size());
        allocInfo.pSetLayouts = layouts.data();

        descriptorSets.resize(engine.SwapChain.SwapChainImages.size());
        if (vkAllocateDescriptorSets(engine.Device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        for (size_t i = 0; i < engine.SwapChain.SwapChainImages.size(); i++) {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = mesh[0].MeshList[0].UniformBuffer.Buffer;
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(SceneDataBufferData);

            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = textureManager.GetTexture(0)->GetTextureView();
            imageInfo.sampler = textureManager.GetTexture(0)->GetTextureSampler();

            std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = descriptorSets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &bufferInfo;

            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = descriptorSets[i];
            descriptorWrites[1].dstBinding = 6;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pImageInfo = &imageInfo;

            vkUpdateDescriptorSets(engine.Device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        }
    }

    void createCommandBuffers() {
        commandBuffers.resize(RenderPass.SwapChainFramebuffers.size());

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
            renderPassInfo.renderPass = RenderPass.RenderPass;
            renderPassInfo.framebuffer = RenderPass.SwapChainFramebuffers[i];
            renderPassInfo.renderArea.offset = { 0, 0 };
            renderPassInfo.renderArea.extent = engine.SwapChain.SwapChainResolution;

            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
            clearValues[1].depthStencil = { 1.0f, 0 };

            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, RenderPass.forwardRendereringPipeline->ShaderPipeline);

            VkBuffer vertexBuffers[] = { mesh[0].MeshList[0].VertexBuffer.Buffer };
            VkDeviceSize offsets[] = { 0 };
            vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);

            vkCmdBindIndexBuffer(commandBuffers[i], mesh[0].MeshList[0].IndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT32);

            vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, RenderPass.forwardRendereringPipeline->ShaderPipelineLayout, 0, 1, &descriptorSets[i], 0, nullptr);

            vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

            vkCmdEndRenderPass(commandBuffers[i]);

            if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to record command buffer!");
            }
        }
    }

    void updateUniformBuffer(uint32_t currentImage) {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto  currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();


        SceneDataBufferData ubo{};
        ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.proj = glm::perspective(glm::radians(45.0f), engine.SwapChain.SwapChainResolution.width / (float)engine.SwapChain.SwapChainResolution.height, 0.1f, 10.0f);
        ubo.proj[1][1] *= -1;

        mesh[0].MeshList[0].UniformBuffer.CopyBufferToMemory(engine.Device, &ubo, sizeof(ubo));
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

        interfaceRenderPass.Draw(engine.Device, imageIndex, engine.SwapChain.SwapChainResolution);
        RayRenderer.updateUniformBuffers(engine, window.GetWindowPtr(), ubo);
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
            CommandBufferSubmitList.emplace_back(interfaceRenderPass.ImGuiCommandBuffers[imageIndex]);
        }
        else
        {
            CommandBufferSubmitList.emplace_back(RayRenderer.drawCmdBuffers[imageIndex]);
            CommandBufferSubmitList.emplace_back(interfaceRenderPass.ImGuiCommandBuffers[imageIndex]);
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

