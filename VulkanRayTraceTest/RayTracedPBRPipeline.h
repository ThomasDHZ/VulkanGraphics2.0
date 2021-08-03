#pragma once
#include "Vulkanengine.h"
#include "AssetManager.h"
#include "RayTracingGraphicsPipeline.h"

class RayTracedPBRPipeline : public RayTracingGraphicsPipeline
{
private:
    void SetUpDescriptorPool(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager);
    void SetUpDescriptorLayout(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager);
    void SetUpDescriptorSets(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, AccelerationStructure& topLevelAS, std::shared_ptr<RenderedRayTracedColorTexture> RayTracedTexture);
    void SetUpPipeline(std::shared_ptr<VulkanEngine> engine);
    void SetUpShaderBindingTable(std::shared_ptr<VulkanEngine> engine);

public:
    RayTracedPBRPipeline();
    RayTracedPBRPipeline(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, AccelerationStructure& topLevelAS, std::shared_ptr<RenderedRayTracedColorTexture> RayTracedTexture);
    ~RayTracedPBRPipeline();

    void UpdateGraphicsPipeLine(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, AccelerationStructure& topLevelAS, std::shared_ptr<RenderedRayTracedColorTexture> RayTracedTexture);
};

