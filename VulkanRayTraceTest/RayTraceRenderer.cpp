#include "RayTraceRenderer.h"

RayTraceRenderer::RayTraceRenderer() : BaseRenderer()
{
}

RayTraceRenderer::RayTraceRenderer(VulkanEngine& engine, VulkanWindow& window, std::shared_ptr<AssetManager> assetManagerPtr) : BaseRenderer(engine, window, assetManagerPtr)
{
    rayTraceRenderPass = RayTraceRenderPass(engine, assetManager);
    //FrameBufferRenderer = FrameBufferRenderPass(engine, assetManager, FrameBufferTextureRenderer.RenderedTexture, FrameBufferTextureRenderer.BloomTexture);
}

RayTraceRenderer::~RayTraceRenderer()
{
}

void RayTraceRenderer::RebuildSwapChain(VulkanEngine& engine, VulkanWindow& window)
{
    rayTraceRenderPass.RebuildSwapChain(engine, assetManager, 0);
    // FrameBufferRenderer.RebuildSwapChain(engine, assetManager, rayTraceRenderPass.storageImage, FrameBufferTextureRenderer.BloomTexture);
}

void RayTraceRenderer::GUIUpdate(VulkanEngine& engine)
{

}

void RayTraceRenderer::Draw(VulkanEngine& engine, VulkanWindow& window, uint32_t imageIndex)
{
    rayTraceRenderPass.Draw(engine, assetManager, imageIndex);
    //  FrameBufferRenderer.Draw(engine, assetManager, imageIndex);
}

void RayTraceRenderer::Destroy(VulkanEngine& engine)
{
    rayTraceRenderPass.Destroy(engine);
    //  FrameBufferRenderer.Destroy(engine);
}

std::vector<VkCommandBuffer> RayTraceRenderer::AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList)
{
    CommandBufferSubmitList.emplace_back(rayTraceRenderPass.RayTraceCommandBuffer);
    //CommandBufferSubmitList.emplace_back(FrameBufferRenderer.CommandBuffer);
    return CommandBufferSubmitList;
}
