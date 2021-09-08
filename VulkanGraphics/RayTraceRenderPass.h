#pragma once
#include "Vulkanengine.h"
//#include "RayTracedHybridPipeline.h"
#include "RayTracedPipeline.h"
//#include "RayTracedPBRPipeline.h"

class RayTraceRenderPass
{
private:
    RayTraceConstants RTXConst;
    VulkanBuffer instancesBuffer;
    void SetUpCommandBuffers(std::shared_ptr<VulkanEngine> engine);

    glm::mat4 LastCameraView = glm::mat4(1.0f);
    glm::mat4 LastCameraProjection = glm::mat4(1.0f);
public:
    uint32_t Frame = 0;

    AccelerationStructure topLevelAS{};
    std::shared_ptr<RenderedRayTracedColorTexture> RayTracedTexture;
    std::shared_ptr<RenderedRayTracedColorTexture> ShadowTextureMask;
    std::shared_ptr<RenderedRayTracedColorTexture> ReflectionTextureMask;
    std::shared_ptr<RenderedRayTracedColorTexture> ReflectionTexture;
    std::shared_ptr<RenderedRayTracedColorTexture> SSAOTexture;

    std::shared_ptr<RayTracedPipeline> RTPipeline;
    //std::shared_ptr<RayTracedPBRPipeline> RTPBRPipeline;
    //std::shared_ptr<RayTracedHybridPipeline> RTHybridPipeline;

    VkCommandBuffer RayTraceCommandBuffer;

    RayTraceRenderPass();
    RayTraceRenderPass(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager);
    ~RayTraceRenderPass();

    void SetUpTopLevelAccelerationStructure(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager);
    void GUIUpdate();
    void Draw(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, RendererID renderID, std::shared_ptr<Camera> ViewCamera);
    void RebuildSwapChain(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager);
    void Destroy(std::shared_ptr<VulkanEngine> engine);

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