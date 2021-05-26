#pragma once
#include "VulkanEngine.h"
#include "RayTracedHybridPipeline.h"
#include "RayTracedPipeline.h"
#include "RayTracedPBRPipeline.h"

class RayTraceRenderPass
{
private:
    VulkanBuffer instancesBuffer;
    void SetUpCommandBuffers(VulkanEngine& engine);
public:

    AccelerationStructure topLevelAS{};
    std::shared_ptr<RenderedRayTracedColorTexture> RayTracedTexture;
    std::shared_ptr<RenderedRayTracedColorTexture> ShadowTextureMask;
    std::shared_ptr<RenderedRayTracedColorTexture> ReflectionTexture;
    std::shared_ptr<RenderedRayTracedColorTexture> SSAOTexture;
    std::shared_ptr<RenderedRayTracedColorTexture> SkyboxTexture;

    std::shared_ptr<RayTracedPipeline> RTPipeline;
    std::shared_ptr<RayTracedPBRPipeline> RTPBRPipeline;
    std::shared_ptr<RayTracedHybridPipeline> RTHybridPipeline;

    VkCommandBuffer RayTraceCommandBuffer;

    RayTraceRenderPass();
    RayTraceRenderPass(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager);
    ~RayTraceRenderPass();

    void SetUpTopLevelAccelerationStructure(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager);
    void Draw(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, uint32_t imageIndex, RendererID renderID, std::shared_ptr<Camera> ViewCamera);
    void RebuildSwapChain(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, uint32_t imageIndex);
    void Destroy(VulkanEngine& engine);

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