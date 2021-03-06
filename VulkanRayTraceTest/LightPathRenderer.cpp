#include "LightPathRenderer.h"

LightPathRenderer::LightPathRenderer() : BaseRenderer()
{
}

LightPathRenderer::LightPathRenderer(VulkanEngine& engine, std::shared_ptr<VulkanWindow> window, std::shared_ptr<AssetManager> assetManagerPtr) : BaseRenderer(engine, window, assetManagerPtr)
{
    lightRenderPass = DepthRenderer(engine, assetManager);
    DebugDepthRenderer = DepthDebugRenderPass(engine, assetManager, lightRenderPass.DepthTexture);
}

LightPathRenderer::~LightPathRenderer()
{
}

void LightPathRenderer::RebuildSwapChain(VulkanEngine& engine, std::shared_ptr<VulkanWindow> window)
{
    lightRenderPass.RebuildSwapChain(engine, assetManager);
    DebugDepthRenderer.RebuildSwapChain(engine, assetManager, lightRenderPass.DepthTexture);
}

void LightPathRenderer::GUIUpdate(VulkanEngine& engine)
{
    ImGui::Image(DebugDepthRenderer.DebugDepthTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
}

void LightPathRenderer::Draw(VulkanEngine& engine, std::shared_ptr<VulkanWindow> window, uint32_t imageIndex)
{
    lightRenderPass.Draw(engine, assetManager, imageIndex);
    DebugDepthRenderer.Draw(engine, assetManager, imageIndex);
}

void LightPathRenderer::Destroy(VulkanEngine& engine)
{
    lightRenderPass.Destroy(engine);
    DebugDepthRenderer.Destroy(engine);
}

std::vector<VkCommandBuffer> LightPathRenderer::AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList)
{
    CommandBufferSubmitList.emplace_back(lightRenderPass.CommandBuffer);
    CommandBufferSubmitList.emplace_back(DebugDepthRenderer.CommandBuffer);
    return CommandBufferSubmitList;
}