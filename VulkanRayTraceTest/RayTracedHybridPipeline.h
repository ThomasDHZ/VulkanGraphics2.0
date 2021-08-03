#pragma once
#include "Vulkanengine.h"
#include "AssetManager.h"
#include "RayTracingGraphicsPipeline.h"

class RayTracedHybridPipeline : public RayTracingGraphicsPipeline
{
private:
    void SetUpDescriptorPool(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager);
    void SetUpDescriptorLayout(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager);
    void SetUpDescriptorSets(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, AccelerationStructure& topLevelAS, std::shared_ptr<RenderedRayTracedColorTexture> ShadowTextureMask, std::shared_ptr<RenderedRayTracedColorTexture> ReflectionMaskTexture, std::shared_ptr<RenderedRayTracedColorTexture> ReflectionTexture, std::shared_ptr<RenderedRayTracedColorTexture> SSAOTexture);
    void SetUpPipeline(std::shared_ptr<VulkanEngine> engine);
    void SetUpShaderBindingTable(std::shared_ptr<VulkanEngine> engine);

public:
    RayTracedHybridPipeline();
    RayTracedHybridPipeline(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, AccelerationStructure& topLevelAS, std::shared_ptr<RenderedRayTracedColorTexture> ShadowTextureMask, std::shared_ptr<RenderedRayTracedColorTexture> ReflectionMaskTexture, std::shared_ptr<RenderedRayTracedColorTexture> ReflectionTexture, std::shared_ptr<RenderedRayTracedColorTexture> SSAOTexture);
    ~RayTracedHybridPipeline();

    void UpdateGraphicsPipeLine(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, AccelerationStructure& topLevelAS, std::shared_ptr<RenderedRayTracedColorTexture> ShadowTextureMask, std::shared_ptr<RenderedRayTracedColorTexture> ReflectionMaskTexture, std::shared_ptr<RenderedRayTracedColorTexture> ReflectionTexture, std::shared_ptr<RenderedRayTracedColorTexture> SSAOTexture);
};

