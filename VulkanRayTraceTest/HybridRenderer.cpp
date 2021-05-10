#include "HybridRenderer.h"

HybridRenderer::HybridRenderer() : BaseRenderer()
{
}

HybridRenderer::HybridRenderer(VulkanEngine& engine, VulkanWindow& window, std::shared_ptr<AssetManager> assetManagerPtr) : BaseRenderer(engine, window, assetManagerPtr)
{
    rayTraceRenderPass = RayTraceRenderPass(engine, assetManager);
    FrameBufferTextureRenderer = FrameBufferTextureRenderPass(engine, assetManager);
    FrameBufferRenderer = HybridFrameBufferRenderPass(engine, assetManager, FrameBufferTextureRenderer.RenderedTexture, rayTraceRenderPass.RayTracedImage, rayTraceRenderPass.RayTracedImage, rayTraceRenderPass.RayTracedImage, rayTraceRenderPass.RayTracedImage);
}

HybridRenderer::~HybridRenderer()
{
}

void HybridRenderer::RebuildSwapChain(VulkanEngine& engine, VulkanWindow& window)
{
    rayTraceRenderPass.RebuildSwapChain(engine, assetManager, 0);
    FrameBufferTextureRenderer.RebuildSwapChain(engine, assetManager);
    FrameBufferRenderer.RebuildSwapChain(engine, assetManager, FrameBufferTextureRenderer.RenderedTexture, rayTraceRenderPass.RayTracedImage, rayTraceRenderPass.RayTracedImage, rayTraceRenderPass.RayTracedImage, rayTraceRenderPass.RayTracedImage);
}

void HybridRenderer::GUIUpdate(VulkanEngine& engine)
{
    ImGui::SliderFloat3("DirectionalLight", &assetManager->SceneData->UniformDataInfo.dlight.direction.x, -1.0f, 1.0f);
    //ImGui::Image(rayTraceRenderPass.RayTracedImage->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
}

void HybridRenderer::Draw(VulkanEngine& engine, VulkanWindow& window, uint32_t imageIndex)
{
    rayTraceRenderPass.Draw(engine, assetManager, imageIndex);
    FrameBufferTextureRenderer.Draw(engine, assetManager, imageIndex, rendererID);
    FrameBufferRenderer.Draw(engine, assetManager, imageIndex, rendererID);
}

void HybridRenderer::Destroy(VulkanEngine& engine)
{
    rayTraceRenderPass.Destroy(engine);
    FrameBufferTextureRenderer.Destroy(engine);
    FrameBufferRenderer.Destroy(engine);
}

std::vector<VkCommandBuffer> HybridRenderer::AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList)
{
    CommandBufferSubmitList.emplace_back(rayTraceRenderPass.RayTraceCommandBuffer);
    CommandBufferSubmitList.emplace_back(FrameBufferTextureRenderer.CommandBuffer);
    CommandBufferSubmitList.emplace_back(FrameBufferRenderer.CommandBuffer);
    return CommandBufferSubmitList;
}
