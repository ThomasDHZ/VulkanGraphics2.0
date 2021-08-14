#include "LightPathRenderer.h"

LightPathRenderer::LightPathRenderer() : BaseRenderer()
{
}

LightPathRenderer::LightPathRenderer(std::shared_ptr<VulkanEngine> engine) : BaseRenderer()
{
    lightRenderPass = DepthRenderer(engine);
    DebugDepthRenderer = DepthDebugRenderPass(lightRenderPass.DepthTexture);
}

LightPathRenderer::~LightPathRenderer()
{
}

void LightPathRenderer::RebuildSwapChain()
{
    lightRenderPass.RebuildSwapChain();
    DebugDepthRenderer.RebuildSwapChain(lightRenderPass.DepthTexture);
}

void LightPathRenderer::GUIUpdate()
{
    ImGui::Image(DebugDepthRenderer.DebugDepthTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
}

void LightPathRenderer::Draw()
{
    lightRenderPass.Draw();
    DebugDepthRenderer.Draw();
}

void LightPathRenderer::Destroy()
{
    lightRenderPass.Destroy();
    DebugDepthRenderer.Destroy();
}

std::vector<VkCommandBuffer> LightPathRenderer::AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList)
{
    CommandBufferSubmitList.emplace_back(lightRenderPass.CommandBuffer);
    CommandBufferSubmitList.emplace_back(DebugDepthRenderer.CommandBuffer);
    return CommandBufferSubmitList;
}