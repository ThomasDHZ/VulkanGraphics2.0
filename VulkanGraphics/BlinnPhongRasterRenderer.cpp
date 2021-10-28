#include "BlinnPhongRasterRenderer.h"


BlinnPhongRasterRenderer::BlinnPhongRasterRenderer() : BaseRenderer()
{
}

BlinnPhongRasterRenderer::BlinnPhongRasterRenderer(std::shared_ptr<VulkanEngine> engine) : BaseRenderer()
{
    DepthRenderPass = DepthPassRendererPass(512);
    depthCubeMapRenderPass = DepthCubeMapRenderPass(1024);
    //DebugDepthRenderPass = DepthDebugRenderPass(DepthRenderPass.DepthTextureList[0]);
    ReflectionRenderPass = ReflectionCubeMapRenderPass(64, nullptr);
    BlinnRenderPass = BlinnPhongRasterPass(engine, DepthRenderPass.DepthTextureList);
    FrameBufferRenderer = FrameBufferRenderPass(BlinnRenderPass.RenderedTexture, BlinnRenderPass.RenderedTexture);
}

BlinnPhongRasterRenderer::~BlinnPhongRasterRenderer()
{
}

void BlinnPhongRasterRenderer::RebuildSwapChain()
{
    DepthRenderPass.RebuildSwapChain(512);
    depthCubeMapRenderPass.RebuildSwapChain(1024);
   // DebugDepthRenderPass.RebuildSwapChain(DepthRenderPass.DepthTextureList[0]);
    ReflectionRenderPass.RebuildSwapChain(1024, DepthRenderPass.DepthTextureList[0]);
    BlinnRenderPass.RebuildSwapChain(DepthRenderPass.DepthTextureList);
    FrameBufferRenderer = FrameBufferRenderPass(BlinnRenderPass.RenderedTexture, BlinnRenderPass.RenderedTexture);
}

void BlinnPhongRasterRenderer::GUIUpdate()
{
    GUIChangedFlag |= ImGui::Checkbox("Shadow Debug", &ShadowDebugFlag);
    ImGui::Checkbox("Cube Debug", &cubeMapCheck);

    if (GUIChangedFlag)
    {
        RebuildSwapChain();
        GUIChangedFlag = false;
    }
}

void BlinnPhongRasterRenderer::Update()
{
    depthCubeMapRenderPass.Update();
    ReflectionRenderPass.Update();
}

void BlinnPhongRasterRenderer::Draw()
{
    if (ShadowDebugFlag)
    {
      //  DebugDepthRenderPass.Draw();
    }

    DepthRenderPass.Draw();
    depthCubeMapRenderPass.Draw();
    ReflectionRenderPass.Draw();
    BlinnRenderPass.Draw();
    FrameBufferRenderer.Draw();
}

void BlinnPhongRasterRenderer::Destroy()
{
   // DebugDepthRenderPass.Destroy();
    depthCubeMapRenderPass.Destroy();
    ReflectionRenderPass.Destroy();
    BlinnRenderPass.Destroy();
    FrameBufferRenderer.Destroy();
}

std::vector<VkCommandBuffer> BlinnPhongRasterRenderer::AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList)
{
    if (ShadowDebugFlag)
    {
      // CommandBufferSubmitList.emplace_back(DebugDepthRenderPass.GetCommandBuffer());
    }

    CommandBufferSubmitList.emplace_back(DepthRenderPass.GetCommandBuffer());
    CommandBufferSubmitList.emplace_back(depthCubeMapRenderPass.GetCommandBuffer());
    CommandBufferSubmitList.emplace_back(ReflectionRenderPass.GetCommandBuffer());
    CommandBufferSubmitList.emplace_back(BlinnRenderPass.GetCommandBuffer());
    CommandBufferSubmitList.emplace_back(FrameBufferRenderer.GetCommandBuffer());
    return CommandBufferSubmitList;
}