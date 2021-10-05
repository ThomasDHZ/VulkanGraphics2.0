#include "PBRRenderer.h"

PBRRenderer::PBRRenderer() : BaseRenderer()
{
}

PBRRenderer::PBRRenderer(std::shared_ptr<VulkanEngine> engine) : BaseRenderer()
{
  //  equirectangularToCubemapRenderPass = EquirectangularToCubemapRenderPass();
   // irradianceRenderPass = IrradianceRenderPass(engine);
    prefilterRenderPass = PrefilterRenderPass(engine);
  //  brdfRenderPass = BRDFRenderPass();

    AssetManagerPtr::GetAssetPtr()->textureManager->LoadCubeMap(prefilterRenderPass.RenderedCubeMap);
   // pbrRenderer = PBRRenderPass(engine, irradianceRenderPass.RenderedCubeMap);
   // FrameBufferRenderer = FrameBufferRenderPass(pbrRenderer.RenderedTexture, pbrRenderer.RenderedTexture);

}

PBRRenderer::~PBRRenderer()
{
}

void PBRRenderer::RebuildSwapChain()
{
    //equirectangularToCubemapRenderPass.RebuildSwapChain();
   // irradianceRenderPass.RebuildSwapChain();
    prefilterRenderPass.RebuildSwapChain();
    //brdfRenderPass.RebuildSwapChain();
 //   pbrRenderer.RebuildSwapChain(irradianceRenderPass.RenderedCubeMap);
  //  FrameBufferRenderer.RebuildSwapChain(pbrRenderer.RenderedTexture, pbrRenderer.RenderedTexture);

}

void PBRRenderer::GUIUpdate()
{
    
}

void PBRRenderer::Draw()
{
    frameCount++;

    //equirectangularToCubemapRenderPass.Draw();
  //  irradianceRenderPass.Draw();
    prefilterRenderPass.Draw();
    //brdfRenderPass.Draw();
  //  pbrRenderer.Draw();
  //  FrameBufferRenderer.Draw();
}

void PBRRenderer::Destroy()
{
    irradianceRenderPass.Destroy();
  //  pbrRenderer.Destroy();
 //   FrameBufferRenderer.Destroy();
}

std::vector<VkCommandBuffer> PBRRenderer::AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList)
{

    //  CommandBufferSubmitList.emplace_back(equirectangularToCubemapRenderPass.GetCommandBuffer());
 //   CommandBufferSubmitList.emplace_back(irradianceRenderPass.GetCommandBuffer());
    CommandBufferSubmitList.emplace_back(prefilterRenderPass.GetCommandBuffer());
    //  CommandBufferSubmitList.emplace_back(brdfRenderPass.GetCommandBuffer());

   // CommandBufferSubmitList.emplace_back(pbrRenderer.GetCommandBuffer());
  //  CommandBufferSubmitList.emplace_back(FrameBufferRenderer.GetCommandBuffer());
    return CommandBufferSubmitList;
}