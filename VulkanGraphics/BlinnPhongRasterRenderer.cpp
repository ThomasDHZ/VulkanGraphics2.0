#include "BlinnPhongRasterRenderer.h"


BlinnPhongRasterRenderer::BlinnPhongRasterRenderer() : BaseRenderer()
{
}

BlinnPhongRasterRenderer::BlinnPhongRasterRenderer(std::shared_ptr<VulkanEngine> engine) : BaseRenderer()
{
    DepthRenderPass = DepthPassRendererPass(512);
    BlinnRenderPass = BlinnPhongRasterPass(engine);
    FrameBufferRenderer = FrameBufferRenderPass(BlinnRenderPass.RenderedTexture, BlinnRenderPass.RenderedTexture);
}

BlinnPhongRasterRenderer::~BlinnPhongRasterRenderer()
{
}

void BlinnPhongRasterRenderer::RebuildSwapChain()
{
    DepthRenderPass.RebuildSwapChain();
    BlinnRenderPass.RebuildSwapChain();
    FrameBufferRenderer.RebuildSwapChain(BlinnRenderPass.RenderedTexture, BlinnRenderPass.RenderedTexture);
}

void BlinnPhongRasterRenderer::GUIUpdate()
{
}

void BlinnPhongRasterRenderer::Draw()
{
    DepthRenderPass.Draw();
    BlinnRenderPass.Draw();
    FrameBufferRenderer.Draw();
}

void BlinnPhongRasterRenderer::Destroy()
{
    DepthRenderPass.Destroy();
    BlinnRenderPass.Destroy();
    FrameBufferRenderer.Destroy();
}

std::vector<VkCommandBuffer> BlinnPhongRasterRenderer::AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList)
{
    CommandBufferSubmitList.emplace_back(DepthRenderPass.GetCommandBuffer());
    CommandBufferSubmitList.emplace_back(BlinnRenderPass.GetCommandBuffer());
    CommandBufferSubmitList.emplace_back(FrameBufferRenderer.GetCommandBuffer());
    return CommandBufferSubmitList;
}