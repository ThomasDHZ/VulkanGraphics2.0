#include "BlinnPhongRasterRenderer.h"


BlinnPhongRasterRenderer::BlinnPhongRasterRenderer() : BaseRenderer()
{
}

BlinnPhongRasterRenderer::BlinnPhongRasterRenderer(std::shared_ptr<VulkanEngine> engine) : BaseRenderer()
{
    DepthRenderPass = DepthPassRendererPass(512);
    depthCubeMapRenderPass = DepthCubeMapRenderPass(1024);
    DebugDepthRenderPass = DepthDebugRenderPass(DepthRenderPass.DepthTexture);
   // debugCubeDepthRenderPass = DepthCubeDebugRenderPass(depthCubeMapRenderPass.RenderedCubeMap);
    BlinnRenderPass = BlinnPhongRasterPass(engine, DepthRenderPass.DepthTexture);
    FrameBufferRenderer = FrameBufferRenderPass(BlinnRenderPass.RenderedTexture, BlinnRenderPass.RenderedTexture);
}

BlinnPhongRasterRenderer::~BlinnPhongRasterRenderer()
{
}

void BlinnPhongRasterRenderer::RebuildSwapChain()
{
    DepthRenderPass.RebuildSwapChain(512);
    depthCubeMapRenderPass.RebuildSwapChain(1024);
    DebugDepthRenderPass.RebuildSwapChain(DepthRenderPass.DepthTexture);
    //debugCubeDepthRenderPass.RebuildSwapChain(depthCubeMapRenderPass.RenderedCubeMap);
    BlinnRenderPass.RebuildSwapChain(DepthRenderPass.DepthTexture);

    if (ShadowDebugFlag)
    {
        FrameBufferRenderer.RebuildSwapChain(DebugDepthRenderPass.DebugTexture, DebugDepthRenderPass.DebugTexture);
    }
    else
    {
        FrameBufferRenderer.RebuildSwapChain(BlinnRenderPass.RenderedTexture, BlinnRenderPass.RenderedTexture);
    }
}

void BlinnPhongRasterRenderer::GUIUpdate()
{
    GUIChangedFlag |= ImGui::Checkbox("Shadow Debug", &ShadowDebugFlag);

    if (GUIChangedFlag)
    {
        RebuildSwapChain();
        GUIChangedFlag = false;
    }
}

void BlinnPhongRasterRenderer::Draw()
{
    if (ShadowDebugFlag)
    {
        DebugDepthRenderPass.Draw();
       // debugCubeDepthRenderPass.Draw();
    }

    DepthRenderPass.Draw();
    depthCubeMapRenderPass.Draw();
    BlinnRenderPass.Draw();
    FrameBufferRenderer.Draw();
}

void BlinnPhongRasterRenderer::Destroy()
{

    DebugDepthRenderPass.Destroy();
    debugCubeDepthRenderPass.Destroy();
    DepthRenderPass.Destroy();
   // depthCubeMapRenderPass.Destroy();
    BlinnRenderPass.Destroy();
    FrameBufferRenderer.Destroy();
}

std::vector<VkCommandBuffer> BlinnPhongRasterRenderer::AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList)
{
    if (ShadowDebugFlag)
    {
        CommandBufferSubmitList.emplace_back(DebugDepthRenderPass.GetCommandBuffer());
      //  CommandBufferSubmitList.emplace_back(debugCubeDepthRenderPass.GetCommandBuffer());
    }

    CommandBufferSubmitList.emplace_back(DepthRenderPass.GetCommandBuffer());
    CommandBufferSubmitList.emplace_back(depthCubeMapRenderPass.GetCommandBuffer());
    CommandBufferSubmitList.emplace_back(BlinnRenderPass.GetCommandBuffer());
    CommandBufferSubmitList.emplace_back(FrameBufferRenderer.GetCommandBuffer());
    return CommandBufferSubmitList;
}