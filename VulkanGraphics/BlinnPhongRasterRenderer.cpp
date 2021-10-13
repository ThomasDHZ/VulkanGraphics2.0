#include "BlinnPhongRasterRenderer.h"


BlinnPhongRasterRenderer::BlinnPhongRasterRenderer() : BaseRenderer()
{
}

BlinnPhongRasterRenderer::BlinnPhongRasterRenderer(std::shared_ptr<VulkanEngine> engine) : BaseRenderer()
{
    DepthRenderPass = DepthPassRendererPass(512);
    DebugDepthRenderPass = DepthDebugRenderPass(DepthRenderPass.DepthTexture);
    BlinnRenderPass = BlinnPhongRasterPass(engine, DepthRenderPass.DepthTexture);
    FrameBufferRenderer = FrameBufferRenderPass(BlinnRenderPass.RenderedTexture, BlinnRenderPass.RenderedTexture);
}

BlinnPhongRasterRenderer::~BlinnPhongRasterRenderer()
{
}

void BlinnPhongRasterRenderer::RebuildSwapChain()
{
    DepthRenderPass.RebuildSwapChain(512);
    DebugDepthRenderPass.RebuildSwapChain(DepthRenderPass.DepthTexture);
    BlinnRenderPass.RebuildSwapChain(DepthRenderPass.DepthTexture);
    FrameBufferRenderer.RebuildSwapChain(BlinnRenderPass.RenderedTexture, BlinnRenderPass.RenderedTexture);
}

void BlinnPhongRasterRenderer::GUIUpdate()
{
}

void BlinnPhongRasterRenderer::Draw()
{
    DepthRenderPass.Draw();
    DebugDepthRenderPass.Draw(DepthRenderPass.DepthTexture);
    BlinnRenderPass.Draw();
    FrameBufferRenderer.Draw();
}

void BlinnPhongRasterRenderer::Destroy()
{
    DepthRenderPass.Destroy();
    DebugDepthRenderPass.Destroy();
    BlinnRenderPass.Destroy();
    FrameBufferRenderer.Destroy();
}

std::vector<VkCommandBuffer> BlinnPhongRasterRenderer::AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList)
{
    CommandBufferSubmitList.emplace_back(DepthRenderPass.GetCommandBuffer());
    CommandBufferSubmitList.emplace_back(DebugDepthRenderPass.GetCommandBuffer());
    CommandBufferSubmitList.emplace_back(BlinnRenderPass.GetCommandBuffer());
    CommandBufferSubmitList.emplace_back(FrameBufferRenderer.GetCommandBuffer());
    return CommandBufferSubmitList;
}