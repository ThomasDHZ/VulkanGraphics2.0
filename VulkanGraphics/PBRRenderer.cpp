#include "PBRRenderer.h"

PBRRenderer::PBRRenderer() : BaseRenderer()
{
}

PBRRenderer::PBRRenderer(std::shared_ptr<VulkanEngine> engine) : BaseRenderer()
{
    equirectangularToCubemapRenderPass = EquirectangularToCubemapRenderPass();
    irradianceRenderPass = IrradianceRenderPass(engine);
    prefilterRenderPass = PrefilterRenderPass();
    brdfRenderPass = BRDFRenderPass();

    AssetManagerPtr::GetAssetPtr()->textureManager->LoadCubeMap(irradianceRenderPass.RenderedCubeMap);
    pbrRenderer = PBRRenderPass(engine);
    FrameBufferRenderer = FrameBufferRenderPass(irradianceRenderPass.RenderedTexture, irradianceRenderPass.RenderedTexture);

}

PBRRenderer::~PBRRenderer()
{
}

void PBRRenderer::RebuildSwapChain()
{
    //equirectangularToCubemapRenderPass.RebuildSwapChain();
    //irradianceRenderPass.RebuildSwapChain();
    //prefilterRenderPass.RebuildSwapChain();
    //brdfRenderPass.RebuildSwapChain();
    pbrRenderer.RebuildSwapChain();
    FrameBufferRenderer.RebuildSwapChain(irradianceRenderPass.RenderedTexture, irradianceRenderPass.RenderedTexture);

}

void PBRRenderer::GUIUpdate()
{
    
}

void PBRRenderer::Draw()
{
    frameCount++;
    if (frameCount % 60 == 0)
    {
        //equirectangularToCubemapRenderPass.Draw();
        //irradianceRenderPass.Draw();
        //prefilterRenderPass.Draw();
        //brdfRenderPass.Draw();
    }
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
    if (frameCount % 60 == 0)
    {
      //  CommandBufferSubmitList.emplace_back(equirectangularToCubemapRenderPass.GetCommandBuffer());
     //   CommandBufferSubmitList.emplace_back(irradianceRenderPass.GetCommandBuffer());
       // CommandBufferSubmitList.emplace_back(prefilterRenderPass.GetCommandBuffer());
      //  CommandBufferSubmitList.emplace_back(brdfRenderPass.GetCommandBuffer());
    }
    CommandBufferSubmitList.emplace_back(pbrRenderer.GetCommandBuffer());
    CommandBufferSubmitList.emplace_back(FrameBufferRenderer.GetCommandBuffer());
    return CommandBufferSubmitList;
}