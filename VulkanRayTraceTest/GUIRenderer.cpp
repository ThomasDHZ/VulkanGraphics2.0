#include "GUIRenderer.h"

GUIRenderer::GUIRenderer() : BaseRenderer()
{
}

GUIRenderer::GUIRenderer(VulkanEngine& engine, std::shared_ptr<VulkanWindow> window, std::shared_ptr<AssetManager> assetManagerPtr) : BaseRenderer(engine, window, assetManagerPtr)
{
    TextRenderer = TextRenderPass(engine, assetManager);
    FrameBufferRenderer = FrameBufferRenderPass(engine, assetManager, TextRenderer.RenderedTexture, TextRenderer.RenderedTexture);
}

GUIRenderer::~GUIRenderer()
{
}

void GUIRenderer::RebuildSwapChain(VulkanEngine& engine, std::shared_ptr<VulkanWindow> window)
{
}

void GUIRenderer::Draw(VulkanEngine& engine, std::shared_ptr<VulkanWindow> window, uint32_t imageIndex)
{
    TextRenderer.Draw(engine, assetManager, imageIndex, rendererID);
    FrameBufferRenderer.Draw(engine, assetManager, imageIndex);
}

void GUIRenderer::Destroy(VulkanEngine& engine)
{
    FrameBufferRenderer.Destroy(engine);
}

std::vector<VkCommandBuffer> GUIRenderer::AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList)
{
    CommandBufferSubmitList.emplace_back(TextRenderer.CommandBuffer);
    CommandBufferSubmitList.emplace_back(FrameBufferRenderer.CommandBuffer);
    return CommandBufferSubmitList;
}