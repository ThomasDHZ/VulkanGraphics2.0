#include "PBRRenderer.h"

PBRRenderer::PBRRenderer() : BaseRenderer()
{
}

PBRRenderer::PBRRenderer(std::shared_ptr<VulkanEngine> engine) : BaseRenderer()
{
   // equirectangularToCubemapRenderPass = EquirectangularToCubemapRenderPass();
    irradianceRenderPass = IrradianceRenderPass(2048.0f);
    prefilterRenderPass = PrefilterRenderPass(2048.0f);
    //brdfRenderPass = BRDFRenderPass();

    AssetManagerPtr::GetAssetPtr()->textureManager->LoadCubeMap(prefilterRenderPass.RenderedCubeMap);
    pbrRenderer = PBRRenderPass(engine, irradianceRenderPass.RenderedCubeMap, prefilterRenderPass.RenderedCubeMap);
    FrameBufferRenderer = FrameBufferRenderPass(pbrRenderer.RenderedTexture, pbrRenderer.RenderedTexture);

}

PBRRenderer::~PBRRenderer()
{
}

void PBRRenderer::RebuildSwapChain()
{
    //equirectangularToCubemapRenderPass.RebuildSwapChain();
    irradianceRenderPass.RebuildSwapChain(2048.0f);
    prefilterRenderPass.RebuildSwapChain(2048.0f);
   // brdfRenderPass.RebuildSwapChain();
    pbrRenderer.RebuildSwapChain(irradianceRenderPass.RenderedCubeMap, prefilterRenderPass.RenderedCubeMap);
    FrameBufferRenderer.RebuildSwapChain(pbrRenderer.RenderedTexture, pbrRenderer.RenderedTexture);

}

void PBRRenderer::GUIUpdate()
{
    
}

void PBRRenderer::Draw()
{
    frameCount++;

   // equirectangularToCubemapRenderPass.Draw();
    irradianceRenderPass.Draw();
    prefilterRenderPass.Draw();
   // brdfRenderPass.Draw();
    pbrRenderer.Draw();
    FrameBufferRenderer.Draw();
}

void PBRRenderer::Destroy()
{
    irradianceRenderPass.Destroy();
    pbrRenderer.Destroy();
    FrameBufferRenderer.Destroy();
}

std::vector<VkCommandBuffer> PBRRenderer::AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList)
{

    //  CommandBufferSubmitList.emplace_back(equirectangularToCubemapRenderPass.GetCommandBuffer());
    CommandBufferSubmitList.emplace_back(irradianceRenderPass.GetCommandBuffer());
    CommandBufferSubmitList.emplace_back(prefilterRenderPass.GetCommandBuffer());
    //  CommandBufferSubmitList.emplace_back(brdfRenderPass.GetCommandBuffer());

    CommandBufferSubmitList.emplace_back(pbrRenderer.GetCommandBuffer());
    CommandBufferSubmitList.emplace_back(FrameBufferRenderer.GetCommandBuffer());
    return CommandBufferSubmitList;
}