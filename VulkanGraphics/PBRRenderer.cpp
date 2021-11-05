#include "PBRRenderer.h"

PBRRenderer::PBRRenderer() : BaseRenderer()
{
}

PBRRenderer::PBRRenderer(std::shared_ptr<VulkanEngine> engine) : BaseRenderer()
{
    DepthRenderPass = DepthPassRendererPass(512);
    depthCubeMapRenderPass = DepthCubeMapRenderPass(1024);
    ////  DebugDepthRenderPass = DepthDebugRenderPass(DepthRenderPass.DepthTextureList[0]);
    // equirectangularToCubemapRenderPass = EquirectangularToCubemapRenderPass();
    irradianceRenderPass = IrradianceRenderPass(TextureManagerPtr::GetTextureManagerPtr()->GetAllCubeMapTextures()[0], CubeMapSamplerSize);
    prefilterRenderPass = PrefilterRenderPass(TextureManagerPtr::GetTextureManagerPtr()->GetAllCubeMapTextures()[0], CubeMapSamplerSize);
    brdfRenderPass = BRDFRenderPass(CubeMapSamplerSize);

    // // AssetManagerPtr::GetAssetPtr()->textureManager->LoadCubeMap(depthCubeMapRenderPass.RenderedCubeMap);
    pbrRenderer = PBRRenderPass(engine, irradianceRenderPass.RenderedCubeMap, prefilterRenderPass.RenderedCubeMap, brdfRenderPass.BRDFMap, DepthRenderPass.DepthTextureList);
    FrameBufferRenderer = FrameBufferRenderPass(pbrRenderer.RenderedTexture, pbrRenderer.RenderedBloomTexture);
}

PBRRenderer::~PBRRenderer()
{
}

void PBRRenderer::RebuildSwapChain()
{
    DepthRenderPass.RebuildSwapChain(512);
    depthCubeMapRenderPass.RebuildSwapChain(1024);
    //// DebugDepthRenderPass.RebuildSwapChain(DepthRenderPass.DepthTextureList[0]);
    // //equirectangularToCubemapRenderPass.RebuildSwapChain();
    irradianceRenderPass.RebuildSwapChain(TextureManagerPtr::GetTextureManagerPtr()->GetAllCubeMapTextures()[0], CubeMapSamplerSize);
    prefilterRenderPass.RebuildSwapChain(TextureManagerPtr::GetTextureManagerPtr()->GetAllCubeMapTextures()[0], CubeMapSamplerSize);
    brdfRenderPass.RebuildSwapChain(CubeMapSamplerSize);
    pbrRenderer.RebuildSwapChain(irradianceRenderPass.RenderedCubeMap, prefilterRenderPass.RenderedCubeMap, brdfRenderPass.BRDFMap, DepthRenderPass.DepthTextureList);
    FrameBufferRenderer.RebuildSwapChain(pbrRenderer.RenderedTexture, pbrRenderer.RenderedBloomTexture);
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
        //   DebugDepthRenderPass.Draw();
    }

    DepthRenderPass.Draw();
    depthCubeMapRenderPass.Draw();
    irradianceRenderPass.Draw();
    prefilterRenderPass.Draw();
    brdfRenderPass.Draw();
    pbrRenderer.Draw();
    FrameBufferRenderer.Draw();
}

void PBRRenderer::Destroy()
{
    DepthRenderPass.Destroy();
    depthCubeMapRenderPass.Destroy();
 //  DebugDepthRenderPass.Destroy();
    irradianceRenderPass.Destroy();
    prefilterRenderPass.Destroy();
    brdfRenderPass.Destroy();
    pbrRenderer.Destroy();
    FrameBufferRenderer.Destroy();
}

std::vector<VkCommandBuffer> PBRRenderer::AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList)
{
    if (ShadowDebugFlag)
    {
        // CommandBufferSubmitList.emplace_back(DebugDepthRenderPass.GetCommandBuffer());
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