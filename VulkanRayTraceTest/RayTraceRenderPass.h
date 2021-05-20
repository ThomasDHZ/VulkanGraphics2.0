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
#include "RayTracedHybridPipeline.h"

class RayTraceRenderPass
{
private:
    VulkanBuffer instancesBuffer;

public:

    AccelerationStructure topLevelAS{};
    std::shared_ptr<RenderedRayTracedColorTexture> ShadowTextureMask;
    std::shared_ptr<RenderedRayTracedColorTexture> ReflectionTexture;
    std::shared_ptr<RenderedRayTracedColorTexture> SSAOTexture;
    std::shared_ptr<RenderedRayTracedColorTexture> SkyboxTexture;

    std::shared_ptr<RayTracedHybridPipeline> RTHybridPipeline;
    VkCommandBuffer RayTraceCommandBuffer;

    RayTraceRenderPass();
    RayTraceRenderPass(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager);
    ~RayTraceRenderPass();

    void createTopLevelAccelerationStructure(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager);
    void Draw(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, uint32_t imageIndex);
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