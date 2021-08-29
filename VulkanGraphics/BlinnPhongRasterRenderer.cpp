#include "BlinnPhongRasterRenderer.h"


BlinnPhongRasterRenderer::BlinnPhongRasterRenderer() : BaseRenderer()
{
}

BlinnPhongRasterRenderer::BlinnPhongRasterRenderer(std::shared_ptr<VulkanEngine> engine) : BaseRenderer()
{
    BlinnRenderPass = BlinnPhongRasterPass(engine);
    FrameBufferRenderer = FrameBufferRenderPass(BlinnRenderPass.RenderedTexture, BlinnRenderPass.BloomTexture);

}

BlinnPhongRasterRenderer::~BlinnPhongRasterRenderer()
{
}

void BlinnPhongRasterRenderer::RebuildSwapChain()
{
    BlinnRenderPass.RebuildSwapChain();
    FrameBufferRenderer.RebuildSwapChain(BlinnRenderPass.RenderedTexture, BlinnRenderPass.BloomTexture);

}

void BlinnPhongRasterRenderer::GUIUpdate()
{
}

void BlinnPhongRasterRenderer::Draw()
{
    BlinnRenderPass.Draw();
    FrameBufferRenderer.Draw();
}

void BlinnPhongRasterRenderer::Destroy()
{
    BlinnRenderPass.Destroy();
    FrameBufferRenderer.Destroy();
}

std::vector<VkCommandBuffer> BlinnPhongRasterRenderer::AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList)
{
    CommandBufferSubmitList.emplace_back(BlinnRenderPass.GetCommandBuffer());
    CommandBufferSubmitList.emplace_back(FrameBufferRenderer.GetCommandBuffer());
    return CommandBufferSubmitList;
}