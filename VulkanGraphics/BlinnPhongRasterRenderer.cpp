#include "BlinnPhongRasterRenderer.h"


BlinnPhongRasterRenderer::BlinnPhongRasterRenderer() : BaseRenderer()
{
}

BlinnPhongRasterRenderer::BlinnPhongRasterRenderer(std::shared_ptr<Mesh> mesh, Texture2D texture, VkBuffer ubo) : BaseRenderer()
{
    BlinnRenderPass = BlinnPhongRasterPass(mesh, texture, ubo);
    FrameBufferRenderer = FrameBufferRenderPass(EnginePtr::GetEnginePtr()->ScreenResoulation, BlinnRenderPass.RenderedTexture, BlinnRenderPass.BloomTexture);

}

BlinnPhongRasterRenderer::~BlinnPhongRasterRenderer()
{
}

void BlinnPhongRasterRenderer::RebuildSwapChain(std::shared_ptr<Mesh> mesh, Texture2D texture, VkBuffer ubo)
{
    BlinnRenderPass.RebuildSwapChain(mesh, texture, ubo);
    FrameBufferRenderer.RebuildSwapChain(EnginePtr::GetEnginePtr()->ScreenResoulation, BlinnRenderPass.RenderedTexture, BlinnRenderPass.BloomTexture);

}

void BlinnPhongRasterRenderer::GUIUpdate()
{
}

void BlinnPhongRasterRenderer::Draw(int currentFrame)
{
    if (EnginePtr::GetEnginePtr()->imagesInFlight[EnginePtr::GetEnginePtr()->DrawFrame] != VK_NULL_HANDLE)
    {
        vkWaitForFences(EnginePtr::GetEnginePtr()->Device, 1, &EnginePtr::GetEnginePtr()->imagesInFlight[EnginePtr::GetEnginePtr()->DrawFrame], VK_TRUE, UINT64_MAX);
    }
    EnginePtr::GetEnginePtr()->imagesInFlight[EnginePtr::GetEnginePtr()->DrawFrame] = EnginePtr::GetEnginePtr()->inFlightFences[currentFrame];

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