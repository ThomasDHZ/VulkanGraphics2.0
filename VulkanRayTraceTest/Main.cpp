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

struct UniformBufferObject {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
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

    VkPhysicalDeviceRayTracingPipelinePropertiesKHR  rayTracingPipelineProperties{};
    VkPhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructureFeatures{};

    VkPhysicalDeviceBufferDeviceAddressFeatures enabledBufferDeviceAddresFeatures{};
    VkPhysicalDeviceRayTracingPipelineFeaturesKHR enabledRayTracingPipelineFeatures{};
    VkPhysicalDeviceAccelerationStructureFeaturesKHR enabledAccelerationStructureFeatures{};

    RayTraceRenderer RayRenderer;

    MainRenderPass renderer;
    std::shared_ptr<Texture2D> texture;

    Mesh mesh;

    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;

    std::vector<VkCommandBuffer> commandBuffers;

    size_t currentFrame = 0;

    InterfaceRenderPass interfaceRenderPass;

    bool framebufferResized = false;

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
        auto app = reinterpret_cast<HelloTriangleApplication*>(glfwGetWindowUserPointer(window));
        app->framebufferResized = true;
    }

    void initVulkan() 
    {
        vulkanWindow = VulkanWindow(1280, 720, "VulkanEngine");
        vulkanEngine = VulkanEngine(vulkanWindow.GetWindowPtr());

        renderer = MainRenderPass(vulkanEngine);
       interfaceRenderPass = InterfaceRenderPass(vulkanEngine, vulkanWindow.GetWindowPtr());
       texture = std::make_shared<Texture2D>(Texture2D(vulkanEngine, "C:/Users/dotha/source/repos/VulkanGraphics/texture/texture.jpg", VK_FORMAT_R8G8B8A8_SRGB, 0));
       mesh.VertexBuffer.CreateBuffer(vulkanEngine.Device, vulkanEngine.PhysicalDevice, vertices.size() * sizeof(Vertex), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertices.data());
       mesh.IndexBuffer.CreateBuffer(vulkanEngine.Device, vulkanEngine.PhysicalDevice, indices.size() * sizeof(uint32_t), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, indices.data());
       mesh.UniformBufferList.CreateBuffer(vulkanEngine.Device, vulkanEngine.PhysicalDevice, sizeof(SceneData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

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
              RayRenderer.UpdateGUI(vulkanEngine);
            }
            ImGui::Render();

            drawFrame();
        }

        vkDeviceWaitIdle(vulkanEngine.Device);
    }

    void cleanupSwapChain() {
    /*    depthImage.Delete(vulkanEngine);

        for (auto framebuffer : swapChainFramebuffers) {
            vkDestroyFramebuffer(vulkanEngine.Device, framebuffer, nullptr);
        }*/

        vkFreeCommandBuffers(vulkanEngine.Device, vulkanEngine.RenderCommandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

        //vkDestroyPipeline(vulkanEngine.Device, graphicsPipeline, nullptr);
       // vkDestroyPipelineLayout(vulkanEngine.Device, pipelineLayout, nullptr);
       /* vkDestroyRenderPass(vulkanEngine.Device, renderPass, nullptr);*/

        for (auto& imageView : vulkanEngine.SwapChain.GetSwapChainImageViews()) 
        {
            vkDestroyImageView(vulkanEngine.Device, imageView, nullptr);
        }

        vkDestroySwapchainKHR(vulkanEngine.Device, vulkanEngine.SwapChain.GetSwapChain(), nullptr);

       
        

        vkDestroyDescriptorPool(vulkanEngine.Device, descriptorPool, nullptr);
    }

    void cleanup() {
        cleanupSwapChain();

     /*   vkDestroySampler(vulkanEngine.Device, textureSampler, nullptr);
        vkDestroyImageView(vulkanEngine.Device, textureImageView, nullptr);

        vkDestroyImage(vulkanEngine.Device, textureImage, nullptr);
        vkFreeMemory(vulkanEngine.Device, textureImageMemory, nullptr);*/

        //vkDestroyDescriptorSetLayout(vulkanEngine.Device, descriptorSetLayout, nullptr);


        //for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        //    vkDestroySemaphore(vulkanEngine.Device, vulkanEngine.vulkanSemaphores[i].RenderCompleteSemaphore, nullptr);
        //    vkDestroySemaphore(vulkanEngine.Device, imageAvailableSemaphores[i], nullptr);
        //    vkDestroyFence(vulkanEngine.Device, inFlightFences[i], nullptr);
        //}

        RayRenderer.Destory(vulkanEngine);
        interfaceRenderPass.Destroy(vulkanEngine);

        vkDestroyCommandPool(vulkanEngine.Device, vulkanEngine.RenderCommandPool, nullptr);

        vkDestroyDevice(vulkanEngine.Device, nullptr);

       

        vkDestroySurfaceKHR(vulkanEngine.Instance, vulkanEngine.Surface, nullptr);
        vkDestroyInstance(vulkanEngine.Instance, nullptr);

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

        cleanupSwapChain();

        vulkanEngine.SwapChain.UpdateSwapChain(vulkanWindow.GetWindowPtr(), vulkanEngine.Device, vulkanEngine.PhysicalDevice, vulkanEngine.Surface);

        createDescriptorPool();
        createDescriptorSets();
        createCommandBuffers();

        interfaceRenderPass.UpdateSwapChain(vulkanEngine);
        RayRenderer.Resize(vulkanEngine, width, height);
    }


    void createDescriptorPool() 
    {
        std::vector<VkDescriptorPoolSize>  DescriptorPoolList = {};

        DescriptorPoolList.emplace_back(mesh.AddDsecriptorPoolBinding(vulkanEngine, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER));
        DescriptorPoolList.emplace_back(mesh.AddDsecriptorPoolBinding(vulkanEngine, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER));

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(DescriptorPoolList.size());
        poolInfo.pPoolSizes = DescriptorPoolList.data();
        poolInfo.maxSets = static_cast<uint32_t>(vulkanEngine.SwapChain.GetSwapChainImageCount());

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

        VkDescriptorBufferInfo PositionInfo = mesh.AddBufferDescriptorInfo(vulkanEngine, mesh.UniformBufferList);
        VkDescriptorImageInfo DiffuseMap = mesh.AddImageDescriptorInfo(vulkanEngine, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, texture);
        
        for (size_t i = 0; i < vulkanEngine.SwapChain.GetSwapChainImageCount(); i++)
        {
            std::vector<VkWriteDescriptorSet> DescriptorList;
            DescriptorList.emplace_back(mesh.AddDescriptorSetBufferInfo(vulkanEngine, 0, descriptorSets[i], PositionInfo));
            DescriptorList.emplace_back(mesh.AddDescriptorSetTextureInfo(vulkanEngine, 1, descriptorSets[i], DiffuseMap));

            vkUpdateDescriptorSets(vulkanEngine.Device, static_cast<uint32_t>(DescriptorList.size()), DescriptorList.data(), 0, nullptr);
        }
    }

    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(vulkanEngine.Device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create buffer!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(vulkanEngine.Device, buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(vulkanEngine.Device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate buffer memory!");
        }

        vkBindBufferMemory(vulkanEngine.Device, buffer, bufferMemory, 0);
    }

    VkCommandBuffer beginSingleTimeCommands() {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = vulkanEngine.RenderCommandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(vulkanEngine.Device, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        return commandBuffer;
    }

    void endSingleTimeCommands(VkCommandBuffer commandBuffer) {
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(vulkanEngine.GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(vulkanEngine.GraphicsQueue);

        vkFreeCommandBuffers(vulkanEngine.Device, vulkanEngine.RenderCommandPool, 1, &commandBuffer);
    }

    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        endSingleTimeCommands(commandBuffer);
    }

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(vulkanEngine.PhysicalDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
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

            vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, renderer.pipeline->ShaderPipeline);

            VkBuffer vertexBuffers[] = { mesh.VertexBuffer.Buffer };
            VkDeviceSize offsets[] = { 0 };
            vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);

            vkCmdBindIndexBuffer(commandBuffers[i], mesh.IndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT32);

            vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, renderer.pipeline->ShaderPipelineLayout, 0, 1, &descriptorSets[i], 0, nullptr);

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


        UniformBufferObject ubo{};
        ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.proj = glm::perspective(glm::radians(45.0f), vulkanEngine.SwapChain.GetSwapChainResolution().width / (float)vulkanEngine.SwapChain.GetSwapChainResolution().height, 0.1f, 10.0f);
        ubo.proj[1][1] *= -1;

        mesh.UniformBufferList.CopyBufferToMemory(vulkanEngine.Device, &ubo, sizeof(ubo));
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
        CommandBufferSubmitList.emplace_back(commandBuffers[imageIndex]);
     // CommandBufferSubmitList.emplace_back(RayRenderer.drawCmdBuffers[imageIndex]);
       // CommandBufferSubmitList.emplace_back(interfaceRenderPass.ImGuiCommandBuffers[imageIndex]);

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

