#pragma once
#include "Vulkanengine.h"
#include "AssetManager.h"
#include "RayTracingGraphicsPipeline.h"

class RayTracedPipeline : public RayTracingGraphicsPipeline
{
private:
    void SetUpDescriptorBindings(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, AccelerationStructure& topLevelAS, std::shared_ptr<RenderedRayTracedColorTexture> RayTracedTexture);
    void SetUpPipeline(std::shared_ptr<VulkanEngine> engine);
    void SetUpShaderBindingTable(std::shared_ptr<VulkanEngine> engine);

public:
    RayTracedPipeline();
    RayTracedPipeline(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, AccelerationStructure& topLevelAS, std::shared_ptr<RenderedRayTracedColorTexture> RayTracedTexture);
    ~RayTracedPipeline();

    void UpdateGraphicsPipeLine(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, AccelerationStructure& topLevelAS, std::shared_ptr<RenderedRayTracedColorTexture> RayTracedTexture);
};

