#pragma once
#include "VulkanEngine.h"
#include "AssetManager.h"
#include "RayTracingGraphicsPipeline.h"

class RayTracedHybridPipeline : public RayTracingGraphicsPipeline
{
private:
    void SetUpDescriptorPool(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager);
    void SetUpDescriptorLayout(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager);
    void SetUpDescriptorSets(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, AccelerationStructure& topLevelAS, std::shared_ptr<RenderedRayTracedColorTexture> ShadowTextureMask, std::shared_ptr<RenderedRayTracedColorTexture> ReflectionMaskTexture, std::shared_ptr<RenderedRayTracedColorTexture> ReflectionTexture, std::shared_ptr<RenderedRayTracedColorTexture> SSAOTexture);
    void SetUpPipeline(VulkanEngine& engine);
    void SetUpShaderBindingTable(VulkanEngine& engine);

public:
    RayTracedHybridPipeline();
    RayTracedHybridPipeline(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, AccelerationStructure& topLevelAS, std::shared_ptr<RenderedRayTracedColorTexture> ShadowTextureMask, std::shared_ptr<RenderedRayTracedColorTexture> ReflectionMaskTexture, std::shared_ptr<RenderedRayTracedColorTexture> ReflectionTexture, std::shared_ptr<RenderedRayTracedColorTexture> SSAOTexture);
    ~RayTracedHybridPipeline();

    void UpdateGraphicsPipeLine(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, AccelerationStructure& topLevelAS, std::shared_ptr<RenderedRayTracedColorTexture> ShadowTextureMask, std::shared_ptr<RenderedRayTracedColorTexture> ReflectionMaskTexture, std::shared_ptr<RenderedRayTracedColorTexture> ReflectionTexture, std::shared_ptr<RenderedRayTracedColorTexture> SSAOTexture);
};

