#include "PBRRenderer.h"

PBRRenderer::PBRRenderer() : BaseRenderer()
{
}

PBRRenderer::PBRRenderer(std::shared_ptr<VulkanEngine> engine) : BaseRenderer()
{
   // equirectangularToCubemapRenderPass = EquirectangularToCubemapRenderPass();
    irradianceRenderPass = IrradianceRenderPass(CubeMapSamplerSize);
    prefilterRenderPass = PrefilterRenderPass(CubeMapSamplerSize);
    brdfRenderPass = BRDFRenderPass(CubeMapSamplerSize);

  //  AssetManagerPtr::GetAssetPtr()->textureManager->LoadCubeMap(TextureManagerPtr::GetTextureManagerPtr()->GetTextureByBufferIndex(0));
    pbrRenderer = PBRRenderPass(engine, irradianceRenderPass.RenderedCubeMap, prefilterRenderPass.RenderedCubeMap, brdfRenderPass.BRDFMap);
    FrameBufferRenderer = FrameBufferRenderPass(pbrRenderer.RenderedTexture, pbrRenderer.RenderedBloomTexture);

}

PBRRenderer::~PBRRenderer()
{
}

void PBRRenderer::RebuildSwapChain()
{
    //equirectangularToCubemapRenderPass.RebuildSwapChain();
    irradianceRenderPass.RebuildSwapChain(CubeMapSamplerSize);
    prefilterRenderPass.RebuildSwapChain(CubeMapSamplerSize);
    brdfRenderPass.RebuildSwapChain(CubeMapSamplerSize);
    pbrRenderer.RebuildSwapChain(irradianceRenderPass.RenderedCubeMap, prefilterRenderPass.RenderedCubeMap, brdfRenderPass.BRDFMap);
    FrameBufferRenderer.RebuildSwapChain(pbrRenderer.RenderedTexture, pbrRenderer.RenderedBloomTexture);

}

void PBRRenderer::GUIUpdate()
{
    
}

void PBRRenderer::Draw()
{
   // equirectangularToCubemapRenderPass.Draw();
    irradianceRenderPass.Draw();
prefilterRenderPass.Draw();
    brdfRenderPass.Draw();
    pbrRenderer.Draw();
    FrameBufferRenderer.Draw();
}

void PBRRenderer::Destroy()
{
  //  irradianceRenderPass.Destroy();
  //  pbrRenderer.Destroy();
    FrameBufferRenderer.Destroy();
}

std::vector<VkCommandBuffer> PBRRenderer::AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList)
{

    //  CommandBufferSubmitList.emplace_back(equirectangularToCubemapRenderPass.GetCommandBuffer());
    CommandBufferSubmitList.emplace_back(irradianceRenderPass.GetCommandBuffer());
    //CommandBufferSubmitList.emplace_back(prefilterRenderPass.GetCommandBuffer());
    CommandBufferSubmitList.emplace_back(brdfRenderPass.GetCommandBuffer());

CommandBufferSubmitList.emplace_back(pbrRenderer.GetCommandBuffer());
    CommandBufferSubmitList.emplace_back(FrameBufferRenderer.GetCommandBuffer());
    return CommandBufferSubmitList;
}