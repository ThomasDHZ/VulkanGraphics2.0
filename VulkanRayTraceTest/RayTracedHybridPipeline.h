#pragma once
#include "VulkanEngine.h"
#include "AssetManager.h"
class RayTracedHybridPipeline
{
private:


    std::vector<VkShaderModule> shaderModules;
    std::vector<VkRayTracingShaderGroupCreateInfoKHR> RayTraceShaders{};

    void SetUpDescriptorPool(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager);
    void SetUpDescriptorLayout(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager);
    void SetUpDescriptorSets(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, AccelerationStructure& topLevelAS, std::shared_ptr<RenderedRayTracedColorTexture> ShadowTextureMask, std::shared_ptr<RenderedRayTracedColorTexture> ReflectionTexture, std::shared_ptr<RenderedRayTracedColorTexture> SSAOTexture, std::shared_ptr<RenderedRayTracedColorTexture> SkyboxTexture);
    void SetUpPipeline(VulkanEngine& engine);
    void SetUpShaderBindingTable(VulkanEngine& engine);
public:
    VulkanBuffer raygenShaderBindingTable;
    VulkanBuffer missShaderBindingTable;
    VulkanBuffer hitShaderBindingTable;

    VkPhysicalDeviceRayTracingPipelinePropertiesKHR  rayTracingPipelineProperties{};
    VkPhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructureFeatures{};

    VkDescriptorPool DescriptorPool = VK_NULL_HANDLE;
    VkDescriptorSetLayout DescriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorSet DescriptorSets = VK_NULL_HANDLE;
    VkPipeline            RayTracePipeline = VK_NULL_HANDLE;
    VkPipelineLayout      RayTracePipelineLayout = VK_NULL_HANDLE;

    RayTracedHybridPipeline();
    RayTracedHybridPipeline(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, AccelerationStructure& topLevelAS, std::shared_ptr<RenderedRayTracedColorTexture> ShadowTextureMask, std::shared_ptr<RenderedRayTracedColorTexture> ReflectionTexture, std::shared_ptr<RenderedRayTracedColorTexture> SSAOTexture, std::shared_ptr<RenderedRayTracedColorTexture> SkyboxTexture);
    ~RayTracedHybridPipeline();

    void RebuildSwapChain(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, AccelerationStructure& topLevelAS, std::shared_ptr<RenderedRayTracedColorTexture> ShadowTextureMask, std::shared_ptr<RenderedRayTracedColorTexture> ReflectionTexture, std::shared_ptr<RenderedRayTracedColorTexture> SSAOTexture, std::shared_ptr<RenderedRayTracedColorTexture> SkyboxTexture);
    void Destroy(VulkanEngine& engine);
};

