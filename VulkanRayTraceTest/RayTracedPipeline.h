#pragma once
#include "VulkanEngine.h"
#include "AssetManager.h"
#include "RayTracingGraphicsPipeline.h"

class RayTracedPipeline : public RayTracingGraphicsPipeline
{
private:
    void SetUpDescriptorPool(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager);
    void SetUpDescriptorLayout(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager);
    void SetUpDescriptorSets(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, AccelerationStructure& topLevelAS, std::shared_ptr<RenderedRayTracedColorTexture> RayTracedTexture);
    void SetUpPipeline(VulkanEngine& engine);
    void SetUpShaderBindingTable(VulkanEngine& engine);

public:
    RayTracedPipeline();
    RayTracedPipeline(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, AccelerationStructure& topLevelAS, std::shared_ptr<RenderedRayTracedColorTexture> RayTracedTexture);
    ~RayTracedPipeline();

    void UpdateGraphicsPipeLine(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, AccelerationStructure& topLevelAS, std::shared_ptr<RenderedRayTracedColorTexture> RayTracedTexture);
};

