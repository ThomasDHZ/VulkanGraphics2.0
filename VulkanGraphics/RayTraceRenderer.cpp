#include "RayTraceRenderer.h"

RayTraceRenderer::RayTraceRenderer() : BaseRenderer()
{
}

RayTraceRenderer::RayTraceRenderer(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window, std::shared_ptr<AssetManager> assetManagerPtr) : BaseRenderer()
{
    assetManager = assetManagerPtr;
    rayTraceRenderPass = RayTraceRenderPass(engine, assetManagerPtr);
    FrameBufferRenderer = FrameBufferRenderPass(rayTraceRenderPass.RayTracedTexture, rayTraceRenderPass.RayTracedTexture);
}

RayTraceRenderer::~RayTraceRenderer()
{
}

void RayTraceRenderer::RebuildSwapChain(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window)
{
    rayTraceRenderPass.RebuildSwapChain(engine, assetManager);
    FrameBufferRenderer.RebuildSwapChain(rayTraceRenderPass.RayTracedTexture, rayTraceRenderPass.RayTracedTexture);
}

void RayTraceRenderer::GUIUpdate(std::shared_ptr<VulkanEngine> engine)
{
    rayTraceRenderPass.GUIUpdate();
}

void RayTraceRenderer::Draw(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window)
{
    rayTraceRenderPass.Draw(engine, assetManager, rendererID, assetManager->cameraManager->ActiveCamera);
    FrameBufferRenderer.Draw();
}

void RayTraceRenderer::Destroy(std::shared_ptr<VulkanEngine> engine)
{
    rayTraceRenderPass.Destroy(engine);
    FrameBufferRenderer.Destroy();
}

std::vector<VkCommandBuffer> RayTraceRenderer::AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList)
{
    CommandBufferSubmitList.emplace_back(rayTraceRenderPass.RayTraceCommandBuffer);
    CommandBufferSubmitList.emplace_back(FrameBufferRenderer.CommandBuffer[EnginePtr::GetEnginePtr()->CMDIndex]);
    return CommandBufferSubmitList;
}
