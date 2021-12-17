#pragma once
#include "Vulkanengine.h"
#include "AssetManager.h"
#include "RayTracingGraphicsPipeline.h"

class PBRRayTracedPipeline : public RayTracingGraphicsPipeline
{
private:
    void SetUpDescriptorBindings(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, AccelerationStructure& topLevelAS, std::shared_ptr<RenderedRayTracedColorTexture> RayTracedTexture);
    void SetUpPipeline(std::shared_ptr<VulkanEngine> engine);
    void SetUpShaderBindingTable(std::shared_ptr<VulkanEngine> engine);

public:
    PBRRayTracedPipeline();
    PBRRayTracedPipeline(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, AccelerationStructure& topLevelAS, std::shared_ptr<RenderedRayTracedColorTexture> RayTracedTexture);
    ~PBRRayTracedPipeline();

    void UpdateGraphicsPipeLine(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, AccelerationStructure& topLevelAS, std::shared_ptr<RenderedRayTracedColorTexture> RayTracedTexture);
};

