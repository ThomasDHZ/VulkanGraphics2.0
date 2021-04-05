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
#include "Model.h"
#include "PerspectiveCamera.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "InterfaceRenderPass.h"
#include "Texture2D.h"
#include "CubeMapTexture.h"
#include "TextureManager.h"
#include "VulkanEngine.h"
#include "RenderedColorTexture.h"
#include "RenderedRayTracedColorTexture.h"
#include "AccelerationStructure.h"
#include "AssetManager.h"

class RayTraceRenderer
{
private:
    VkDescriptorPool DescriptorPool = VK_NULL_HANDLE;
    VkDescriptorSetLayout DescriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorSet DescriptorSets = VK_NULL_HANDLE;
    VkPipeline            RayTracePipeline = VK_NULL_HANDLE;
    VkPipelineLayout      RayTracePipelineLayout = VK_NULL_HANDLE;

    std::vector<VkShaderModule> shaderModules;
    VulkanBuffer instancesBuffer;

    VkPhysicalDeviceBufferDeviceAddressFeatures enabledBufferDeviceAddresFeatures{};
    VkPhysicalDeviceRayTracingPipelineFeaturesKHR enabledRayTracingPipelineFeatures{};
    VkPhysicalDeviceAccelerationStructureFeaturesKHR enabledAccelerationStructureFeatures{};

    VkPhysicalDeviceRayTracingPipelinePropertiesKHR  rayTracingPipelineProperties{};
    VkPhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructureFeatures{};

    std::vector<VkRayTracingShaderGroupCreateInfoKHR> RayTraceShaders{};
    VulkanBuffer raygenShaderBindingTable;
    VulkanBuffer missShaderBindingTable;
    VulkanBuffer hitShaderBindingTable;

    void SetUpDescriptorPool(VulkanEngine& engine, AssetManager& assetManager);
    void SetUpDescriptorLayout(VulkanEngine& engine, AssetManager& assetManager);
    void SetUpDescriptorSets(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> sceneData);
public:

    AccelerationStructure topLevelAS{};
    std::shared_ptr<RenderedRayTracedColorTexture> storageImage;
    VkCommandBuffer RayTraceCommandBuffer;

    RayTraceRenderer();
    RayTraceRenderer(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> sceneData);
    ~RayTraceRenderer();

    void createTopLevelAccelerationStructure(VulkanEngine& engine, AssetManager& assetManager);
    void createStorageImage(VulkanEngine& engine);
    void createRayTracingPipeline(VulkanEngine& engine);
    void createShaderBindingTable(VulkanEngine& engine);
    void buildCommandBuffers(VulkanEngine& engine, AssetManager& assetManager, uint32_t imageIndex);
    void Resize(VulkanEngine& engine, AssetManager& assetManager, std::shared_ptr<SceneDataUniformBuffer> sceneData, uint32_t imageIndex);
    void Destory(VulkanEngine& engine);

    VkTransformMatrixKHR GLMToVkTransformMatrix(glm::mat4 matrix)
    {
        return VkTransformMatrixKHR
        {
            matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3],
            matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3],
            matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3],
        };
    }
};