#pragma once
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
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
#include "VulkanBuffer.h"
#include <vector>
#include "Buffer.h"
#include "RayTraceModel.h"
#include "PerspectiveCamera.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "InterfaceRenderPass.h"

struct CubeMapLayout
{
    std::string Front;
    std::string Back;
    std::string Top;
    std::string Bottom;
    std::string Right;
    std::string Left;
};

struct RayTracingScratchBuffer
{
    uint64_t deviceAddress = 0;
    VkBuffer handle = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
};

struct StorageImage {
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkImage image = VK_NULL_HANDLE;
    VkImageView view = VK_NULL_HANDLE;
    VkFormat format;
};

struct AccelerationStructure {
    VkAccelerationStructureKHR handle = VK_NULL_HANDLE;
    uint64_t deviceAddress = 0;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkBuffer buffer = VK_NULL_HANDLE;

    //void Destory(VkDevice& device)
    //{
    //    vkFreeMemory(device, memory, nullptr);
    //    vkDestroyBuffer(device, buffer, nullptr);
    //    vkDestroyAccelerationStructureKHR(device, handle, nullptr);

    //    memory = VK_NULL_HANDLE;
    //    buffer = VK_NULL_HANDLE;
    //    handle = VK_NULL_HANDLE;
    //    deviceAddress = 0;
    //}
};

struct Texture
{
    VkImage textureImage = VK_NULL_HANDLE;
    VkDeviceMemory textureImageMemory = VK_NULL_HANDLE;
    VkImageView textureImageView = VK_NULL_HANDLE;
    VkSampler textureSampler = VK_NULL_HANDLE;
};

class RayTraceRenderer
{
private:
    VkDevice device;
    VkPhysicalDevice physicalDevice;
    VkCommandPool commandPool;
    VkQueue graphicsQueue;
    uint32_t WIDTH;
    uint32_t HEIGHT;

    PFN_vkGetBufferDeviceAddressKHR vkGetBufferDeviceAddressKHR;
    PFN_vkCreateAccelerationStructureKHR vkCreateAccelerationStructureKHR;
    PFN_vkDestroyAccelerationStructureKHR vkDestroyAccelerationStructureKHR;
    PFN_vkGetAccelerationStructureBuildSizesKHR vkGetAccelerationStructureBuildSizesKHR;
    PFN_vkGetAccelerationStructureDeviceAddressKHR vkGetAccelerationStructureDeviceAddressKHR;
    PFN_vkCmdBuildAccelerationStructuresKHR vkCmdBuildAccelerationStructuresKHR;
    PFN_vkBuildAccelerationStructuresKHR vkBuildAccelerationStructuresKHR;
    PFN_vkCmdTraceRaysKHR vkCmdTraceRaysKHR;
    PFN_vkGetRayTracingShaderGroupHandlesKHR vkGetRayTracingShaderGroupHandlesKHR;
    PFN_vkCreateRayTracingPipelinesKHR vkCreateRayTracingPipelinesKHR;

    VkPhysicalDeviceBufferDeviceAddressFeatures enabledBufferDeviceAddresFeatures{};
    VkPhysicalDeviceRayTracingPipelineFeaturesKHR enabledRayTracingPipelineFeatures{};
    VkPhysicalDeviceAccelerationStructureFeaturesKHR enabledAccelerationStructureFeatures{};

    VkPhysicalDeviceRayTracingPipelinePropertiesKHR  rayTracingPipelineProperties{};
    VkPhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructureFeatures{};

    StorageImage storageImage;

    VkPipeline            RayTracePipeline = VK_NULL_HANDLE;
    VkPipelineLayout      RayTracePipelineLayout = VK_NULL_HANDLE;
    VkDescriptorSet       RTDescriptorSet = VK_NULL_HANDLE;
    VkDescriptorSetLayout RayTraceDescriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorPool      descriptorPool = VK_NULL_HANDLE;

    std::vector<VkRayTracingShaderGroupCreateInfoKHR> RayTraceShaders{};
    VulkanBuffer raygenShaderBindingTable;
    VulkanBuffer missShaderBindingTable;
    VulkanBuffer hitShaderBindingTable;

    UniformData SceneData;
    VulkanBuffer SceneDataBuffer;

public:

    std::shared_ptr<PerspectiveCamera> camera;
    Keyboard keyboard;
    Mouse mouse;

    Texture DiffuseMap;
    Texture CubeMap;

    std::vector<AccelerationStructure> bottomLevelASList{};
    AccelerationStructure topLevelAS{};

    std::vector<VkShaderModule> shaderModules;
    std::vector<VkCommandBuffer> drawCmdBuffers;

    RayTraceRenderer();
    RayTraceRenderer(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, VkDescriptorPool descriptorPool, uint32_t WIDTH, uint32_t HEIGHT, int swapChainFramebuffersSize, std::vector<VkImage>& swapChainImages);
    ~RayTraceRenderer();

    void Destory();

    RayTraceModel model;
    RayTraceModel model2;

    std::vector<VulkanBuffer> VertexBufferList;
    std::vector<VulkanBuffer> IndexBufferList;
    VulkanBuffer POSBuffer;
    VulkanBuffer UVBuffer;
    VulkanBuffer NormalBuffer;

    void createBottomLevelAccelerationStructure(RayTraceModel& model, Mesh& mesh);
    void createTopLevelAccelerationStructure();
    void createStorageImage();
    void createRayTracingPipeline();
    void createShaderBindingTable();
    void createSceneDataBuffer();
    void createDescriptorSets();
    void buildCommandBuffers(int swapChainFramebuffersSize, std::vector<VkImage>& swapChainImages);
    void Resize(int swapChainFramebuffersSize, std::vector<VkImage>& swapChainImages, uint32_t width, uint32_t height);

    void AcclerationCommandBuffer(VkAccelerationStructureBuildGeometryInfoKHR& VkAccelerationStructureBuildGeometryInfoKHR, std::vector<VkAccelerationStructureBuildRangeInfoKHR>& accelerationStructureBuildRangeInfoKHR);

    void UpdateGUI();
    void updateUniformBuffers(GLFWwindow* window);

    VkResult createBuffer(VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, Buffer* buffer, VkDeviceSize size, void* data);
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void deleteScratchBuffer(RayTracingScratchBuffer& scratchBuffer);
    void createAccelerationStructure(AccelerationStructure& accelerationStructure, VkAccelerationStructureTypeKHR type, VkAccelerationStructureBuildSizesInfoKHR buildSizeInfo);
    RayTracingScratchBuffer createScratchBuffer(VkDeviceSize size);
    uint64_t getBufferDeviceAddress(VkBuffer buffer);
    uint32_t getMemoryType(uint32_t typeBits, VkMemoryPropertyFlags properties);
    VkCommandBuffer createCommandBuffer(VkCommandBufferLevel level, VkCommandPool pool, bool begin);
    VkCommandBuffer createCommandBuffer(VkCommandBufferLevel level, bool begin);
    void setImageLayout(
        VkCommandBuffer cmdbuffer,
        VkImage image,
        VkImageLayout oldImageLayout,
        VkImageLayout newImageLayout,
        VkImageSubresourceRange subresourceRange,
        VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
        VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
    VkPipelineShaderStageCreateInfo loadShader(std::string fileName, VkShaderStageFlagBits stage);
    VkShaderModule loadShader(const char* fileName, VkDevice device);
    uint32_t alignedSize(uint32_t value, uint32_t alignment);

    VkTransformMatrixKHR GLMToVkTransformMatrix(glm::mat4 matrix)
    {
        return VkTransformMatrixKHR
        {
            matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3],
            matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3],
            matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3],
        };
    }


    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    uint64_t getBufferDeviceAddress(VkDevice& device, VkBuffer buffer);
    //Texture
    void createTextureImage(Texture& texture, const std::string Filepath);
    void createTextureImageView(Texture& texture);
    void createTextureSampler(Texture& texture);
    //CubeMap Texture
    void CubeMapTexture(std::string CubeMapFiles[6], Texture& CubeMap);
    void LoadTexture(CubeMapLayout CubeMapFiles, Texture& CubeMap);
    void CreateTextureImage(VkImageCreateInfo TextureInfo, Texture& CubeMap);
    void CreateTextureView(Texture& CubeMap);
    void CreateTextureSampler(Texture& CubeMap);

    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
    void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, int levelCount);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, int layerCount);
};