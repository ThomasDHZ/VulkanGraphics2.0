#include "PBRRayTraceRenderer.h"

PBRRayTraceRenderer::PBRRayTraceRenderer() : BaseRenderer()
{
}

PBRRayTraceRenderer::PBRRayTraceRenderer(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window, std::shared_ptr<AssetManager> assetManagerPtr) : BaseRenderer()
{
    assetManager = assetManagerPtr;
    pbrRayTraceRenderPass = PBRRayTraceRenderPass(engine, assetManagerPtr);
    FrameBufferRenderer = FrameBufferRenderPass(pbrRayTraceRenderPass.RayTracedTexture, pbrRayTraceRenderPass.RayTracedTexture);
}

PBRRayTraceRenderer::~PBRRayTraceRenderer()
{
}

void PBRRayTraceRenderer::RebuildSwapChain(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window)
{
    pbrRayTraceRenderPass.RebuildSwapChain(engine, assetManager);
    FrameBufferRenderer.RebuildSwapChain(pbrRayTraceRenderPass.RayTracedTexture, pbrRayTraceRenderPass.RayTracedTexture);
}

void PBRRayTraceRenderer::GUIUpdate(std::shared_ptr<VulkanEngine> engine)
{
    pbrRayTraceRenderPass.GUIUpdate();
}

void PBRRayTraceRenderer::Draw(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window)
{
    pbrRayTraceRenderPass.Draw(engine, assetManager, rendererID, assetManager->cameraManager->ActiveCamera);
    FrameBufferRenderer.Draw();
}

void PBRRayTraceRenderer::Destroy(std::shared_ptr<VulkanEngine> engine)
{
    pbrRayTraceRenderPass.Destroy(engine);
    FrameBufferRenderer.Destroy();
}

std::vector<VkCommandBuffer> PBRRayTraceRenderer::AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList)
{
    CommandBufferSubmitList.emplace_back(pbrRayTraceRenderPass.RayTraceCommandBuffer);
    CommandBufferSubmitList.emplace_back(FrameBufferRenderer.CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex]);
    return CommandBufferSubmitList;
}
