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
#include "Texuture2D.h"
#include "CubeMapTexture.h"
#include "TextureManager.h"
#include "VulkanEngine.h"
#include "RenderedColorTexture.h"
#include "RenderedRayTracedColorTexture.h"

struct RayTracingScratchBuffer
{
    uint64_t deviceAddress = 0;
    VkBuffer handle = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
};

//struct StorageImage {
//    VkDeviceMemory memory = VK_NULL_HANDLE;
//    VkImage image = VK_NULL_HANDLE;
//    VkImageView view = VK_NULL_HANDLE;
//    VkFormat format;
//};

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

class RayTraceRenderer
{
private:
    TextureManager textureManager;

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


    std::vector<VkRayTracingShaderGroupCreateInfoKHR> RayTraceShaders{};
    VulkanBuffer raygenShaderBindingTable;
    VulkanBuffer missShaderBindingTable;
    VulkanBuffer hitShaderBindingTable;

    VkWriteDescriptorSetAccelerationStructureKHR AddAcclerationStructureBinding(VulkanEngine& engine, VkAccelerationStructureKHR& handle);
    VkDescriptorPoolSize AddDsecriptorPoolBinding(VulkanEngine& engine, VkDescriptorType descriptorType);
    VkDescriptorImageInfo AddRayTraceReturnImageDescriptor(VulkanEngine& engine, VkImageLayout ImageLayout, RenderedRayTracedColorTexture texture);
    std::vector<VkDescriptorImageInfo> AddTextureDescriptor(VulkanEngine& engine, VkImageLayout ImageLayout);
    VkDescriptorImageInfo AddTextureDescriptor(VulkanEngine& engine, VkImageLayout ImageLayout, std::shared_ptr<Texture> texture);
    VkDescriptorBufferInfo AddStorageDescriptor(VulkanEngine& engine, VulkanBuffer buffer);
    std::vector<VkDescriptorBufferInfo> AddStorageDescriptor(VulkanEngine& engine, std::vector<VulkanBuffer> BufferList);
    VkDescriptorBufferInfo AddBufferDescriptor(VulkanEngine& engine, VkBuffer Buffer, VkDeviceSize BufferSize);
    std::vector<VkDescriptorBufferInfo> AddVertexBufferListDescriptor();
    std::vector<VkDescriptorBufferInfo> AddIndexBufferListDescriptor();

    VkWriteDescriptorSet AddAccelerationBuffer(VulkanEngine& engine, unsigned int BindingNumber, VkWriteDescriptorSetAccelerationStructureKHR& accelerationStructure);
    VkWriteDescriptorSet AddStorageBuffer(VulkanEngine& engine, unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, VkDescriptorBufferInfo& BufferInfo);
    VkWriteDescriptorSet AddStorageBuffer(VulkanEngine& engine, unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, std::vector<VkDescriptorBufferInfo>& BufferInfoList);
    VkWriteDescriptorSet AddStorageImageBuffer(VulkanEngine& engine, unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, VkDescriptorImageInfo& BufferInfo);
    VkWriteDescriptorSet AddDescriptorSetBuffer(VulkanEngine& engine, unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, VkDescriptorBufferInfo& BufferInfo);
    VkWriteDescriptorSet AddDescriptorSetTexture(VulkanEngine& engine, unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, VkDescriptorImageInfo& TextureImageInfo);
    VkWriteDescriptorSet AddDescriptorSetTexture(VulkanEngine& engine, unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, std::vector<VkDescriptorImageInfo>& TextureImageInfo);

public:

    RenderedRayTracedColorTexture storageImage;
    RenderedRayTracedColorTexture shadowStorageImage;

    VkPipeline            RayTracePipeline = VK_NULL_HANDLE;
    VkPipelineLayout      RayTracePipelineLayout = VK_NULL_HANDLE;
    VkDescriptorSet       RTDescriptorSet = VK_NULL_HANDLE;
    VkDescriptorSetLayout RayTraceDescriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorPool      descriptorPool = VK_NULL_HANDLE;

    std::vector<AccelerationStructure> bottomLevelASList{};
    AccelerationStructure topLevelAS{};

    std::vector<VkShaderModule> shaderModules;
    std::vector<VkCommandBuffer> drawCmdBuffers;

    RayTraceRenderer();
    RayTraceRenderer(VulkanEngine& engine, TextureManager& textureManagerz, std::vector<RayTraceModel>& modelList, VkDescriptorPool& descpool);
    ~RayTraceRenderer();

    void Destory(VulkanEngine& engine);

    std::vector<RayTraceModel> ModelList;

    std::vector<VulkanBuffer> VertexBufferList;
    std::vector<VulkanBuffer> IndexBufferList;
    VulkanBuffer MaterialBuffer;

    void createBottomLevelAccelerationStructure(VulkanEngine& engine, RayTraceModel& model, Mesh& mesh);
    void createTopLevelAccelerationStructure(VulkanEngine& engine);
    void createRayTracingPipeline(VulkanEngine& engine);
    void createShaderBindingTable(VulkanEngine& engine);
    void createSceneDataBuffer(VulkanEngine& engine);
    void buildCommandBuffers(VulkanEngine& engine, int swapChainFramebuffersSize, std::vector<VkImage>& swapChainImages);
    void Resize(VulkanEngine& engine, int swapChainFramebuffersSize, std::vector<VkImage>& swapChainImages, uint32_t width, uint32_t height);

    void AcclerationCommandBuffer(VulkanEngine& engine, VkAccelerationStructureBuildGeometryInfoKHR& VkAccelerationStructureBuildGeometryInfoKHR, std::vector<VkAccelerationStructureBuildRangeInfoKHR>& accelerationStructureBuildRangeInfoKHR);

    VkResult createBuffer(VulkanEngine& engine, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, Buffer* buffer, VkDeviceSize size, void* data);
    void createBuffer(VulkanEngine& engine, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void deleteScratchBuffer(VulkanEngine& engine, RayTracingScratchBuffer& scratchBuffer);
    void createAccelerationStructure(VulkanEngine& engine, AccelerationStructure& accelerationStructure, VkAccelerationStructureTypeKHR type, VkAccelerationStructureBuildSizesInfoKHR buildSizeInfo);
    RayTracingScratchBuffer createScratchBuffer(VulkanEngine& engine, VkDeviceSize size);
    uint64_t getBufferDeviceAddress(VulkanEngine& engine, VkBuffer buffer);
    uint32_t getMemoryType(VulkanEngine& engine, uint32_t typeBits, VkMemoryPropertyFlags properties);
    VkCommandBuffer createCommandBuffer(VulkanEngine& engine, VkCommandBufferLevel level, VkCommandPool pool, bool begin);
    VkCommandBuffer createCommandBuffer(VulkanEngine& engine, VkCommandBufferLevel level, bool begin);
    VkPipelineShaderStageCreateInfo loadShader(VulkanEngine& engine, std::string fileName, VkShaderStageFlagBits stage);
    VkShaderModule loadShader(VulkanEngine& engine, const char* fileName, VkDevice device);
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


    VkCommandBuffer beginSingleTimeCommands(VulkanEngine& engine);
    void endSingleTimeCommands(VulkanEngine& engine, VkCommandBuffer commandBuffer);
    uint32_t findMemoryType(VulkanEngine& engine, uint32_t typeFilter, VkMemoryPropertyFlags properties);
    uint64_t getBufferDeviceAddress(VkDevice& device, VkBuffer buffer);
};