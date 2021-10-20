#include "PBRRenderer.h"

PBRRenderer::PBRRenderer() : BaseRenderer()
{
}

PBRRenderer::PBRRenderer(std::shared_ptr<VulkanEngine> engine) : BaseRenderer()
{
    DepthRenderPass = DepthPassRendererPass(512);
    depthCubeMapRenderPass = DepthCubeMapRenderPass(1024);
    DebugDepthRenderPass = DepthDebugRenderPass(DepthRenderPass.DepthTexture);
    // debugCubeDepthRenderPass = DepthCubeDebugRenderPass(depthCubeMapRenderPass.RenderedCubeMap);
   // equirectangularToCubemapRenderPass = EquirectangularToCubemapRenderPass();
    irradianceRenderPass = IrradianceRenderPass(CubeMapSamplerSize);
    prefilterRenderPass = PrefilterRenderPass(CubeMapSamplerSize);
    brdfRenderPass = BRDFRenderPass(CubeMapSamplerSize);

  //  AssetManagerPtr::GetAssetPtr()->textureManager->LoadCubeMap(TextureManagerPtr::GetTextureManagerPtr()->GetTextureByBufferIndex(0));
    pbrRenderer = PBRRenderPass(engine, irradianceRenderPass.RenderedCubeMap, prefilterRenderPass.RenderedCubeMap, brdfRenderPass.BRDFMap, DepthRenderPass.DepthTexture);
    FrameBufferRenderer = FrameBufferRenderPass(pbrRenderer.RenderedTexture, pbrRenderer.RenderedBloomTexture);

}

PBRRenderer::~PBRRenderer()
{
}

void PBRRenderer::RebuildSwapChain()
{
    DepthRenderPass.RebuildSwapChain(512);
    depthCubeMapRenderPass.RebuildSwapChain(1024);
    DebugDepthRenderPass.RebuildSwapChain(DepthRenderPass.DepthTexture);
    //debugCubeDepthRenderPass.RebuildSwapChain(depthCubeMapRenderPass.RenderedCubeMap);
    //equirectangularToCubemapRenderPass.RebuildSwapChain();
    irradianceRenderPass.RebuildSwapChain(CubeMapSamplerSize);
    prefilterRenderPass.RebuildSwapChain(CubeMapSamplerSize);
    brdfRenderPass.RebuildSwapChain(CubeMapSamplerSize);
    pbrRenderer.RebuildSwapChain(irradianceRenderPass.RenderedCubeMap, prefilterRenderPass.RenderedCubeMap, brdfRenderPass.BRDFMap, DepthRenderPass.DepthTexture);
    FrameBufferRenderer.RebuildSwapChain(pbrRenderer.RenderedTexture, pbrRenderer.RenderedBloomTexture);

    if (ShadowDebugFlag)
    {
        FrameBufferRenderer.RebuildSwapChain(DebugDepthRenderPass.DebugTexture, DebugDepthRenderPass.DebugTexture);
    }
    else
    {
        FrameBufferRenderer.RebuildSwapChain(pbrRenderer.RenderedTexture, pbrRenderer.RenderedTexture);
    }
}

void PBRRenderer::GUIUpdate()
{
    GUIChangedFlag |= ImGui::Checkbox("Shadow Debug", &ShadowDebugFlag);

    if (GUIChangedFlag)
    {
        RebuildSwapChain();
        GUIChangedFlag = false;
    }
}

void PBRRenderer::Draw()
{
    if (ShadowDebugFlag)
    {
        DebugDepthRenderPass.Draw();
        // debugCubeDepthRenderPass.Draw();
    }

    DepthRenderPass.Draw();
    depthCubeMapRenderPass.Draw();
   // equirectangularToCubemapRenderPass.Draw();
    irradianceRenderPass.Draw();
prefilterRenderPass.Draw();
    brdfRenderPass.Draw();
    pbrRenderer.Draw();
    FrameBufferRenderer.Draw();
}

void PBRRenderer::Destroy()
{
    DebugDepthRenderPass.Destroy();
    debugCubeDepthRenderPass.Destroy();
    irradianceRenderPass.Destroy();
    prefilterRenderPass.Destroy();
    brdfRenderPass.Destroy();
    irradianceRenderPass.Destroy();
    pbrRenderer.Destroy();
    FrameBufferRenderer.Destroy();
}

std::vector<VkCommandBuffer> PBRRenderer::AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList)
{
    if (ShadowDebugFlag)
    {
        CommandBufferSubmitList.emplace_back(DebugDepthRenderPass.GetCommandBuffer());
        //  CommandBufferSubmitList.emplace_back(debugCubeDepthRenderPass.GetCommandBuffer());
    }

    CommandBufferSubmitList.emplace_back(DepthRenderPass.GetCommandBuffer());
    CommandBufferSubmitList.emplace_back(depthCubeMapRenderPass.GetCommandBuffer());
    //  CommandBufferSubmitList.emplace_back(equirectangularToCubemapRenderPass.GetCommandBuffer());
    CommandBufferSubmitList.emplace_back(irradianceRenderPass.GetCommandBuffer());
    //CommandBufferSubmitList.emplace_back(prefilterRenderPass.GetCommandBuffer());
    CommandBufferSubmitList.emplace_back(brdfRenderPass.GetCommandBuffer());

CommandBufferSubmitList.emplace_back(pbrRenderer.GetCommandBuffer());
    CommandBufferSubmitList.emplace_back(FrameBufferRenderer.GetCommandBuffer());
    return CommandBufferSubmitList;
}