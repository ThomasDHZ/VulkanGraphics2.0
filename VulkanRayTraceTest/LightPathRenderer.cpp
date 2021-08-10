#include "LightPathRenderer.h"

LightPathRenderer::LightPathRenderer() : BaseRenderer()
{
}

LightPathRenderer::LightPathRenderer(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window, std::shared_ptr<AssetManager> assetManagerPtr) : BaseRenderer(engine, window, assetManagerPtr)
{
    lightRenderPass = DepthRenderer(engine, assetManager);
    DebugDepthRenderer = DepthDebugRenderPass(lightRenderPass.DepthTexture);
}

LightPathRenderer::~LightPathRenderer()
{
}

void LightPathRenderer::RebuildSwapChain(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window)
{
    lightRenderPass.RebuildSwapChain(engine, assetManager);
    DebugDepthRenderer.RebuildSwapChain(lightRenderPass.DepthTexture);
}

void LightPathRenderer::GUIUpdate(std::shared_ptr<VulkanEngine> engine)
{
    ImGui::Image(DebugDepthRenderer.DebugDepthTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
}

void LightPathRenderer::Draw(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window)
{
    lightRenderPass.Draw(engine, assetManager);
    DebugDepthRenderer.Draw();
}

void LightPathRenderer::Destroy(std::shared_ptr<VulkanEngine> engine)
{
    lightRenderPass.Destroy(engine);
    DebugDepthRenderer.Destroy();
}

std::vector<VkCommandBuffer> LightPathRenderer::AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList)
{
    CommandBufferSubmitList.emplace_back(lightRenderPass.CommandBuffer);
    CommandBufferSubmitList.emplace_back(DebugDepthRenderer.CommandBuffer);
    return CommandBufferSubmitList;
}