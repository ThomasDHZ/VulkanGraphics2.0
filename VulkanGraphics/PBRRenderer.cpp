#include "PBRRenderer.h"

PBRRenderer::PBRRenderer() : BaseRenderer()
{
}

PBRRenderer::PBRRenderer(std::shared_ptr<VulkanEngine> engine) : BaseRenderer()
{
    pbrRenderer = PBRRenderPass(engine);
    FrameBufferRenderer = FrameBufferRenderPass(pbrRenderer.RenderedTexture, pbrRenderer.RenderedTexture);

}

PBRRenderer::~PBRRenderer()
{
}

void PBRRenderer::RebuildSwapChain()
{
    pbrRenderer.RebuildSwapChain();
    FrameBufferRenderer.RebuildSwapChain(pbrRenderer.RenderedTexture, pbrRenderer.RenderedTexture);

}

void PBRRenderer::GUIUpdate()
{
}

void PBRRenderer::Draw()
{
    pbrRenderer.Draw();
    FrameBufferRenderer.Draw();
}

void PBRRenderer::Destroy()
{
    pbrRenderer.Destroy();
    FrameBufferRenderer.Destroy();
}

std::vector<VkCommandBuffer> PBRRenderer::AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList)
{
    CommandBufferSubmitList.emplace_back(pbrRenderer.GetCommandBuffer());
    CommandBufferSubmitList.emplace_back(FrameBufferRenderer.GetCommandBuffer());
    return CommandBufferSubmitList;
}