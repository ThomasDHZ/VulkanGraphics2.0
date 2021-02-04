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
#include "VulkanEngine.h"
#include "InterfaceRenderPass.h"
#include "RayTraceRenderer.h"
#include "ForwardRenderingPipeline.h"
#include "MainRenderPass.h"
#include "Mesh.h"

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
    VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
    VK_KHR_MAINTENANCE3_EXTENSION_NAME,
    VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME,
    VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
    VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
    VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
    VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
    VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
    VK_KHR_SPIRV_1_4_EXTENSION_NAME,
    VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

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
    VulkanWindow vulkanWindow;
    VulkanEngine vulkanEngine;

    RayTraceRenderer RayRenderer;

    MainRenderPass renderer;
    std::shared_ptr<Texture2D> texture;
    std::shared_ptr<Texture2D> texture2;

    Mesh mesh;
    Mesh mesh2;
    int TextureID = 0;
    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;

    std::vector<VkCommandBuffer> commandBuffers;

    size_t currentFrame = 0;

    InterfaceRenderPass interfaceRenderPass;

    bool RayTracingFlag = true;
    bool framebufferResized = false;

    void initVulkan() 
    {
        vulkanWindow = VulkanWindow(1280, 720, "VulkanEngine");
        vulkanEngine = VulkanEngine(vulkanWindow.GetWindowPtr());

        renderer = MainRenderPass(vulkanEngine);
       interfaceRenderPass = InterfaceRenderPass(vulkanEngine, vulkanWindow.GetWindowPtr());
       texture = std::make_shared<Texture2D>(Texture2D(vulkanEngine, "C:/Users/dotha/source/repos/VulkanGraphics/texture/texture.jpg", VK_FORMAT_R8G8B8A8_SRGB, 0));
       texture2 = std::make_shared<Texture2D>(Texture2D(vulkanEngine, "C:/Users/dotha/source/repos/VulkanGraphics/texture/Brick_diffuseOriginal.bmp", VK_FORMAT_R8G8B8A8_SRGB, 0));
       mesh.VertexBuffer.CreateBuffer(vulkanEngine.Device, vulkanEngine.PhysicalDevice, vertices.size() * sizeof(Vertex), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertices.data());
       mesh.IndexBuffer.CreateBuffer(vulkanEngine.Device, vulkanEngine.PhysicalDevice, indices.size() * sizeof(uint32_t), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, indices.data());
       mesh.UniformBuffer.CreateBuffer(vulkanEngine.Device, vulkanEngine.PhysicalDevice, sizeof(SceneData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

       mesh2.VertexBuffer.CreateBuffer(vulkanEngine.Device, vulkanEngine.PhysicalDevice, vertices.size() * sizeof(Vertex), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertices.data());
       mesh2.IndexBuffer.CreateBuffer(vulkanEngine.Device, vulkanEngine.PhysicalDevice, indices.size() * sizeof(uint32_t), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, indices.data());
       mesh2.UniformBuffer.CreateBuffer(vulkanEngine.Device, vulkanEngine.PhysicalDevice, sizeof(SceneData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        createDescriptorPool();
        createDescriptorSets();
        createCommandBuffers();

        RayRenderer = RayTraceRenderer(vulkanEngine, descriptorPool);
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(vulkanWindow.GetWindowPtr())) {
            glfwPollEvents();

            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            {
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                ImGui::Checkbox("RayTrace", &RayTracingFlag);
                ImGui::SliderInt("TextureID", &TextureID, 0, 1);
                RayRenderer.UpdateGUI(vulkanEngine);
            }
            ImGui::Render();

            drawFrame();
        }

        vkDeviceWaitIdle(vulkanEngine.Device);
    }

    void cleanup() {
        RayRenderer.Destory(vulkanEngine);
        interfaceRenderPass.Destroy(vulkanEngine);

        texture->Delete(vulkanEngine);
        mesh.Destory(vulkanEngine);
        renderer.Destroy(vulkanEngine);

        vkDestroyDescriptorPool(vulkanEngine.Device, descriptorPool, nullptr);

        vulkanEngine.Destory();
        vulkanWindow.CleanUp();
    }

    void recreateSwapChain() {
        int width = 0, height = 0;
        glfwGetFramebufferSize(vulkanWindow.GetWindowPtr(), &width, &height);
        while (width == 0 || height == 0) {
            glfwGetFramebufferSize(vulkanWindow.GetWindowPtr(), &width, &height);
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(vulkanEngine.Device);
        
        for (auto imageView : vulkanEngine.SwapChain.GetSwapChainImageViews()) {
            vkDestroyImageView(vulkanEngine.Device, imageView, nullptr);
        }

        vkDestroySwapchainKHR(vulkanEngine.Device, vulkanEngine.SwapChain.GetSwapChain(), nullptr);
        vkDestroyDescriptorPool(vulkanEngine.Device, descriptorPool, nullptr);

        vulkanEngine.SwapChain.UpdateSwapChain(vulkanWindow.GetWindowPtr(), vulkanEngine.Device, vulkanEngine.PhysicalDevice, vulkanEngine.Surface);
        renderer.UpdateSwapChain(vulkanEngine);

        createDescriptorPool();
        createDescriptorSets();
        createCommandBuffers();

        interfaceRenderPass.UpdateSwapChain(vulkanEngine);
        RayRenderer.Resize(vulkanEngine, width, height);
    }


    void createDescriptorPool() 
    {
        std::array<VkDescriptorPoolSize, 2> poolSizes{};
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(renderer.SwapChainFramebuffers.size());
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(renderer.SwapChainFramebuffers.size());

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(renderer.SwapChainFramebuffers.size());

        if (vkCreateDescriptorPool(vulkanEngine.Device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    void createDescriptorSets() 
    {
        std::vector<VkDescriptorSetLayout> layouts(vulkanEngine.SwapChain.GetSwapChainImageCount(), renderer.pipeline->ShaderPipelineDescriptorLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(vulkanEngine.SwapChain.GetSwapChainImageCount());
        allocInfo.pSetLayouts = layouts.data();

        descriptorSets.resize(vulkanEngine.SwapChain.GetSwapChainImageCount());
        if (vkAllocateDescriptorSets(vulkanEngine.Device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        for (size_t i = 0; i < renderer.SwapChainFramebuffers.size(); i++) {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = mesh.UniformBuffer.Buffer;
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(UniformBufferObject);

            VkDescriptorBufferInfo bufferInfo2{};
            bufferInfo2.buffer = mesh2.UniformBuffer.Buffer;
            bufferInfo2.offset = 0;
            bufferInfo2.range = sizeof(UniformBufferObject);

            std::vector<VkDescriptorBufferInfo> bufferList;
            bufferList.emplace_back(bufferInfo);
            bufferList.emplace_back(bufferInfo2);

            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = texture->GetTextureView();
            imageInfo.sampler = texture->GetTextureSampler();

            VkDescriptorImageInfo imageInfo2{};
            imageInfo2.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo2.imageView = texture2->GetTextureView();
            imageInfo2.sampler = texture2->GetTextureSampler();

            std::vector<VkDescriptorImageInfo> infoList;
            infoList.emplace_back(imageInfo);
            infoList.emplace_back(imageInfo2);

            std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = descriptorSets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = static_cast<uint32_t>(bufferList.size());
            descriptorWrites[0].pBufferInfo = bufferList.data();

            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = descriptorSets[i];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[1].descriptorCount = static_cast<uint32_t>(infoList.size());
            descriptorWrites[1].pImageInfo = infoList.data();

            vkUpdateDescriptorSets(vulkanEngine.Device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        }
    }

    void createCommandBuffers() {
        commandBuffers.resize(renderer.SwapChainFramebuffers.size());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = vulkanEngine.RenderCommandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

        if (vkAllocateCommandBuffers(vulkanEngine.Device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
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
            renderPassInfo.renderPass = renderer.RenderPass;
            renderPassInfo.framebuffer = renderer.SwapChainFramebuffers[i];
            renderPassInfo.renderArea.offset = { 0, 0 };
            renderPassInfo.renderArea.extent = vulkanEngine.SwapChain.GetSwapChainResolution();

            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
            clearValues[1].depthStencil = { 1.0f, 0 };

            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            {
            struct MeshInfo
            {
                int MeshID = 0;
                int MaterialID = 0;
            } Mesh;
            Mesh.MeshID = 0;
            Mesh.MaterialID = 0;

            VkBuffer vertexBuffers[] = { mesh.VertexBuffer.Buffer };
            VkDeviceSize offsets[] = { 0 };

            vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, renderer.pipeline->ShaderPipeline);
            vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);
            vkCmdBindIndexBuffer(commandBuffers[i], mesh.IndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT32);
            vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, renderer.pipeline->ShaderPipelineLayout, 0, 1, &descriptorSets[i], 0, nullptr);
            vkCmdPushConstants(commandBuffers[i], renderer.pipeline->ShaderPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(MeshInfo), &Mesh);
            vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
        }
            {
                struct MeshInfo
                {
                    int MeshID = 0;
                    int MaterialID = 0;
                } Mesh2;
                Mesh2.MeshID = 1;
                Mesh2.MaterialID = 1;

                VkBuffer vertexBuffers[] = { mesh2.VertexBuffer.Buffer };
                VkDeviceSize offsets[] = { 0 };

                vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, renderer.pipeline->ShaderPipeline);
                vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);
                vkCmdBindIndexBuffer(commandBuffers[i], mesh2.IndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT32);
                vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, renderer.pipeline->ShaderPipelineLayout, 0, 1, &descriptorSets[i], 0, nullptr);
                vkCmdPushConstants(commandBuffers[i], renderer.pipeline->ShaderPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(MeshInfo), &Mesh2);
                vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
            }





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


        UniformBufferObject ubo{};
        ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.proj = glm::perspective(glm::radians(45.0f), vulkanEngine.SwapChain.GetSwapChainResolution().width / (float)vulkanEngine.SwapChain.GetSwapChainResolution().height, 0.1f, 10.0f);
        ubo.proj[1][1] *= -1;

        mesh.UniformBuffer.CopyBufferToMemory(vulkanEngine.Device, &ubo, sizeof(ubo));


        UniformBufferObject ubo2{};
        ubo2.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, .0f));
        ubo2.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo2.proj = glm::perspective(glm::radians(45.0f), vulkanEngine.SwapChain.GetSwapChainResolution().width / (float)vulkanEngine.SwapChain.GetSwapChainResolution().height, 0.1f, 10.0f);
        ubo2.proj[1][1] *= -1;

        mesh2.UniformBuffer.CopyBufferToMemory(vulkanEngine.Device, &ubo2, sizeof(ubo2));
    }

    void drawFrame() {
        vkWaitForFences(vulkanEngine.Device, 1, &vulkanEngine.inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(vulkanEngine.Device, vulkanEngine.SwapChain.GetSwapChain(), UINT64_MAX, vulkanEngine.vulkanSemaphores[currentFrame].ImageAcquiredSemaphore, VK_NULL_HANDLE, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        interfaceRenderPass.Draw(vulkanEngine, imageIndex);
        RayRenderer.updateUniformBuffers(vulkanEngine, vulkanWindow.GetWindowPtr());
        updateUniformBuffer(imageIndex);

        if (vulkanEngine.imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
            vkWaitForFences(vulkanEngine.Device, 1, &vulkanEngine.imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
        }
        vulkanEngine.imagesInFlight[imageIndex] = vulkanEngine.inFlightFences[currentFrame];

        VkSemaphore waitSemaphores[] = { vulkanEngine.vulkanSemaphores[currentFrame].ImageAcquiredSemaphore };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

        std::vector<VkCommandBuffer> CommandBufferSubmitList;
        
        if (!RayTracingFlag)
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

        VkSemaphore signalSemaphores[] = { vulkanEngine.vulkanSemaphores[currentFrame].RenderCompleteSemaphore };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;


        vkResetFences(vulkanEngine.Device, 1, &vulkanEngine.inFlightFences[currentFrame]);

        if (vkQueueSubmit(vulkanEngine.GraphicsQueue, 1, &submitInfo, vulkanEngine.inFlightFences[currentFrame]) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = { vulkanEngine.SwapChain.GetSwapChain() };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;

        presentInfo.pImageIndices = &imageIndex;

        result = vkQueuePresentKHR(vulkanEngine.PresentQueue, &presentInfo);

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

