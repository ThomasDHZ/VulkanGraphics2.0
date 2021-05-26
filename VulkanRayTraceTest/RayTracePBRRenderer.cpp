#include "RayTracePBRRenderer.h"

RayTracePBRRenderer::RayTracePBRRenderer() : BaseRenderer()
{
}

RayTracePBRRenderer::RayTracePBRRenderer(VulkanEngine& engine, VulkanWindow& window, std::shared_ptr<AssetManager> assetManagerPtr) : BaseRenderer(engine, window, assetManagerPtr)
{
    rayTraceRenderPass = RayTraceRenderPass(engine, assetManager);
    FrameBufferRenderer = FrameBufferRenderPass(engine, assetManager, rayTraceRenderPass.RayTracedTexture, rayTraceRenderPass.RayTracedTexture);
}

RayTracePBRRenderer::~RayTracePBRRenderer()
{
}

void RayTracePBRRenderer::RebuildSwapChain(VulkanEngine& engine, VulkanWindow& window)
{
    rayTraceRenderPass.RebuildSwapChain(engine, assetManager, 0);
    FrameBufferRenderer.RebuildSwapChain(engine, assetManager, rayTraceRenderPass.RayTracedTexture, rayTraceRenderPass.RayTracedTexture);
}

void RayTracePBRRenderer::GUIUpdate(VulkanEngine& engine)
{
   ImGui::Image(rayTraceRenderPass.RayTracedTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
}

void RayTracePBRRenderer::Draw(VulkanEngine& engine, VulkanWindow& window, uint32_t imageIndex)
{
    rayTraceRenderPass.Draw(engine, assetManager, imageIndex, rendererID, assetManager->cameraManager.ActiveCamera);
    FrameBufferRenderer.Draw(engine, assetManager, imageIndex);
}

void RayTracePBRRenderer::Destroy(VulkanEngine& engine)
{
    rayTraceRenderPass.Destroy(engine);
    FrameBufferRenderer.Destroy(engine);
}

std::vector<VkCommandBuffer> RayTracePBRRenderer::AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList)
{
    CommandBufferSubmitList.emplace_back(rayTraceRenderPass.RayTraceCommandBuffer);
    CommandBufferSubmitList.emplace_back(FrameBufferRenderer.CommandBuffer);
    return CommandBufferSubmitList;
}
