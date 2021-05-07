#include "BlinnPhongRasterRenderer.h"

BlinnPhongRasterRenderer::BlinnPhongRasterRenderer() : BaseRenderer()
{
}

BlinnPhongRasterRenderer::BlinnPhongRasterRenderer(VulkanEngine& engine, VulkanWindow& window, std::shared_ptr<AssetManager> assetManagerPtr) : BaseRenderer(engine, window, assetManagerPtr)
{
    FrameBufferTextureRenderer = FrameBufferTextureRenderPass(engine, assetManager);
    FrameBufferRenderer = FrameBufferRenderPass(engine, assetManager, FrameBufferTextureRenderer.RenderedTexture, FrameBufferTextureRenderer.BloomTexture);
    forwardRenderPass = ForwardRenderPass(engine, assetManager);
}

BlinnPhongRasterRenderer::~BlinnPhongRasterRenderer()
{
}

void BlinnPhongRasterRenderer::RebuildSwapChain(VulkanEngine& engine, VulkanWindow& window)
{
    FrameBufferTextureRenderer.RebuildSwapChain(engine, assetManager);
    FrameBufferRenderer.RebuildSwapChain(engine, assetManager, FrameBufferTextureRenderer.RenderedTexture, FrameBufferTextureRenderer.BloomTexture);
    forwardRenderPass.RebuildSwapChain(engine, assetManager);
}

void BlinnPhongRasterRenderer::GUIUpdate(VulkanEngine& engine)
{
    ImGui::Checkbox("FrameBuffer", &UseFrameBuffer);
    ImGui::Image(FrameBufferTextureRenderer.RenderedTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    ImGui::Image(FrameBufferTextureRenderer.BloomTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
}

void BlinnPhongRasterRenderer::Draw(VulkanEngine& engine, VulkanWindow& window, uint32_t imageIndex)
{
    if (UseFrameBuffer)
    {
        FrameBufferTextureRenderer.Draw(engine, assetManager, imageIndex);
        FrameBufferRenderer.Draw(engine, assetManager, imageIndex);
    }
    else
    {
        forwardRenderPass.Draw(engine, assetManager, imageIndex, rendererID);
    }
}

void BlinnPhongRasterRenderer::Destroy(VulkanEngine& engine)
{
    FrameBufferTextureRenderer.Destroy(engine);
    FrameBufferRenderer.Destroy(engine);
    forwardRenderPass.Destroy(engine);
}

std::vector<VkCommandBuffer> BlinnPhongRasterRenderer::AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList)
{
    if (UseFrameBuffer)
    {
        CommandBufferSubmitList.emplace_back(FrameBufferTextureRenderer.CommandBuffer);
        CommandBufferSubmitList.emplace_back(FrameBufferRenderer.CommandBuffer);
    }
    else
    {
         CommandBufferSubmitList.emplace_back(forwardRenderPass.CommandBuffer);
    }
    return CommandBufferSubmitList;
}
