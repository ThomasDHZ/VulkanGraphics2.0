#include "BlinnPhongRasterRenderer.h"


BlinnPhongRasterRenderer::BlinnPhongRasterRenderer() : BaseRenderer()
{
}

BlinnPhongRasterRenderer::BlinnPhongRasterRenderer(std::shared_ptr<VulkanEngine> engine) : BaseRenderer()
{
    DepthRenderPass = DepthPassRendererPass(4096);
    depthCubeMapRenderPass = DepthCubeMapRenderPass(1024);
    DepthSpotLightRenderPass = SpotLightDepthRenderPass(4096);
   //// DebugDepthRenderPass = DepthDebugRenderPass(DepthRenderPass.DepthTextureList[0]);
  //  ReflectionRenderPass = ReflectionCubeMapRenderPass(64, DepthRenderPass.DepthTextureList);
    BlinnRenderPass = BlinnPhongRasterPass(engine, DepthRenderPass.DepthTextureList, depthCubeMapRenderPass.RenderedCubeMap, DepthSpotLightRenderPass.DepthTextureList);
    FrameBufferRenderer = FrameBufferRenderPass(BlinnRenderPass.RenderedTexture, BlinnRenderPass.RenderedTexture);
}

BlinnPhongRasterRenderer::~BlinnPhongRasterRenderer()
{
}

void BlinnPhongRasterRenderer::RebuildSwapChain()
{
    DepthRenderPass.RebuildSwapChain(512);
    depthCubeMapRenderPass.RebuildSwapChain(1024);
    DepthSpotLightRenderPass.RebuildSwapChain(512);
    ////DebugDepthRenderPass.RebuildSwapChain(DepthRenderPass.DepthTextureList[0]);
//    ReflectionRenderPass.RebuildSwapChain(1024, DepthRenderPass.DepthTextureList);
    BlinnRenderPass.RebuildSwapChain(DepthRenderPass.DepthTextureList, depthCubeMapRenderPass.RenderedCubeMap, DepthSpotLightRenderPass.DepthTextureList);
    FrameBufferRenderer.RebuildSwapChain(BlinnRenderPass.RenderedTexture, BlinnRenderPass.RenderedTexture);
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
 //   ReflectionRenderPass.Update();
}

void BlinnPhongRasterRenderer::Draw()
{
    if (ShadowDebugFlag)
    {
      //  DebugDepthRenderPass.Draw();
    }

    DepthRenderPass.Draw();
    depthCubeMapRenderPass.Draw();
    DepthSpotLightRenderPass.Draw();
  //  ReflectionRenderPass.Draw();
    BlinnRenderPass.Draw();
    FrameBufferRenderer.Draw();
}

void BlinnPhongRasterRenderer::Destroy()
{
    DepthRenderPass.Destroy();
   // DebugDepthRenderPass.Destroy();
    depthCubeMapRenderPass.Destroy();
    DepthSpotLightRenderPass.Destroy();
 //   ReflectionRenderPass.Destroy();
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
    CommandBufferSubmitList.emplace_back(DepthSpotLightRenderPass.GetCommandBuffer());
    //CommandBufferSubmitList.emplace_back(ReflectionRenderPass.GetCommandBuffer());
    CommandBufferSubmitList.emplace_back(BlinnRenderPass.GetCommandBuffer());
    CommandBufferSubmitList.emplace_back(FrameBufferRenderer.GetCommandBuffer());
    return CommandBufferSubmitList;
}