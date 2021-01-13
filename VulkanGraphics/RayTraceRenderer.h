#pragma once
#include "VulkanBuffer.h"
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
#include "RayTraceTexture.h"
#include "PerspectiveCamera.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "VulkanWindow.h"
#include "Model.h"

struct RayTracingScratchBuffer
{
    uint64_t deviceAddress = 0;
    VkBuffer handle = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
};

struct AccelerationStructure {
    VkAccelerationStructureKHR handle;
    uint64_t deviceAddress = 0;
    VkDeviceMemory memory;
    VkBuffer buffer;
};

struct StorageImage {
    VkDeviceMemory memory;
    VkImage image;
    VkImageView view;
    VkFormat format;
};

struct UniformData {
    glm::mat4 viewInverse;
    glm::mat4 projInverse;
    glm::mat4 modelInverse;
    glm::vec4 lightPos;
    glm::vec4 viewPos;
    int32_t vertexSize;
};

class RayTraceRenderer
{
private:
    Model model;
    Model model2;
    VkPhysicalDeviceRayTracingPipelinePropertiesKHR  rayTracingPipelineProperties{};
    VkPhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructureFeatures{};

public:

    RayTraceTexture rayTexture;
    Texture2D texture2D;
    Texture2D NormalMap;

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

    std::shared_ptr<PerspectiveCamera> camera;
    Keyboard keyboard;
    Mouse mouse;


    VkPhysicalDeviceBufferDeviceAddressFeatures enabledBufferDeviceAddresFeatures{};
    VkPhysicalDeviceRayTracingPipelineFeaturesKHR enabledRayTracingPipelineFeatures{};
    VkPhysicalDeviceAccelerationStructureFeaturesKHR enabledAccelerationStructureFeatures{};

    VulkanBuffer BottomLevelAccelerationBuffer;
    VulkanBuffer TopLevelAccelerationBuffer;
    VkAccelerationStructureKHR BottomLevelAccelerationStructure;
    VkAccelerationStructureKHR TopLevelAccelerationStructure;
    uint64_t BottomLevelAccelerationDeviceAddress;
    uint64_t TopLevelAccelerationDeviceAddress;

    VulkanBuffer bottomLevelAS;
    VulkanBuffer bottomLevelAS2;
    VulkanBuffer topLevelAS;

    VulkanBuffer vertexBuffer;
    VulkanBuffer indexBuffer;
    VulkanBuffer vertexBuffer2;
    VulkanBuffer indexBuffer2;
    uint32_t indexCount;
    VulkanBuffer transformBuffer;
    VulkanBuffer transformBuffer2;
    std::vector<VkRayTracingShaderGroupCreateInfoKHR> RayTraceShaders{};
    VulkanBuffer raygenShaderBindingTable;
    VulkanBuffer missShaderBindingTable;
    VulkanBuffer hitShaderBindingTable;

   // StorageImage storageImage;
    std::vector<uint32_t> maxPrimCount;
    std::vector<VkAccelerationStructureBuildRangeInfoKHR> AcclerationBuildRangeList;
    std::vector<VkAccelerationStructureGeometryKHR> GeometryAccelerationStructureList;
    UniformData uniformData;
    VulkanBuffer ubo;

    VkPipeline            RayTracePipeline;
    VkPipelineLayout      RayTracePipelineLayout;
    VkDescriptorSet       RTDescriptorSet;
    VkDescriptorSetLayout RayTraceDescriptorSetLayout;
    VkDescriptorPool      descriptorPool;

    std::vector<VkShaderModule> shaderModules;
    std::vector<VkCommandBuffer> drawCmdBuffers;

    RayTraceRenderer();
    RayTraceRenderer(VulkanEngine& engine);
    ~RayTraceRenderer();

    void AddMesh(VulkanEngine& engine);
    void createBottomLevelAccelerationStructure(VulkanEngine& engine);
    void createTopLevelAccelerationStructure(VulkanEngine& engine);
    void createStorageImage(VulkanEngine& engine);
    void createUniformBuffer(VulkanEngine& engine);
    void createRayTracingPipeline(VulkanEngine& engine);
    void createShaderBindingTable(VulkanEngine& engine);
    void createDescriptorSets(VulkanEngine& engine);
    void buildCommandBuffers(VulkanEngine& engine);
    void Resize(VulkanEngine& engine);
    void Destory(VulkanEngine& engine);

    void updateUniformBuffers(VulkanEngine& engine, GLFWwindow* window);
    uint32_t getMemoryType(VulkanEngine& engine, uint32_t typeBits, VkMemoryPropertyFlags properties);
    //void setImageLayout(VulkanEngine& engine,
    //    VkCommandBuffer cmdbuffer,
    //    VkImage image,
    //    VkImageLayout oldImageLayout,
    //    VkImageLayout newImageLayout,
    //    VkImageSubresourceRange subresourceRange,
    //    VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
    //    VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
    VkPipelineShaderStageCreateInfo loadShader(VulkanEngine& engine, std::string fileName, VkShaderStageFlagBits stage);
    VkShaderModule loadShader(const char* fileName, VkDevice device);

    struct Vertex2 {
        glm::vec3 pos = glm::vec3(0.0f);
        glm::vec3 normal = glm::vec3(0.0f);
        glm::vec2 uv = glm::vec2(0.0f);
        glm::vec3 tangent = glm::vec3(0.0f);
        glm::vec4 color = glm::vec4(0.0f);
        glm::vec4 joint0 = glm::vec4(0.0f);
        glm::vec4 weight0 = glm::vec4(0.0f);
        glm::vec4 Padding = glm::vec4(0.0f);
    };
    std::vector<Vertex> CalcVertex();
};

