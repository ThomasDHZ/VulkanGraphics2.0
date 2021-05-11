#include "HybridRenderer.h"

HybridRenderer::HybridRenderer() : BaseRenderer()
{
}

HybridRenderer::HybridRenderer(VulkanEngine& engine, VulkanWindow& window, std::shared_ptr<AssetManager> assetManagerPtr) : BaseRenderer(engine, window, assetManagerPtr)
{
    FrameBufferTextureRenderer = GBufferRenderPass(engine, assetManager);
    rayTraceRenderPass = RayTraceRenderPass(engine, assetManager);

    SSAOTextureList textures = {};
    textures.GPositionTexture = FrameBufferTextureRenderer.GPositionTexture;
    textures.GNormalTexture = FrameBufferTextureRenderer.GNormalTexture;
    SSAORenderer = SSAORenderPass(engine, assetManager, textures);
    FrameBufferRenderer = HybridFrameBufferRenderPass(engine, assetManager, FrameBufferTextureRenderer.GAlbedoTexture, rayTraceRenderPass.ShadowTextureMask, rayTraceRenderPass.SkyboxTexture, FrameBufferTextureRenderer.GAlbedoTexture, FrameBufferTextureRenderer.GAlbedoTexture);
}

HybridRenderer::~HybridRenderer()
{
}

void HybridRenderer::RebuildSwapChain(VulkanEngine& engine, VulkanWindow& window)
{
    FrameBufferTextureRenderer.RebuildSwapChain(engine, assetManager);
    rayTraceRenderPass.RebuildSwapChain(engine, assetManager, 0);

    SSAOTextureList textures = {};
    textures.GPositionTexture = FrameBufferTextureRenderer.GPositionTexture;
    textures.GNormalTexture = FrameBufferTextureRenderer.GNormalTexture;
    SSAORenderer = SSAORenderPass(engine, assetManager, textures);
    FrameBufferRenderer.RebuildSwapChain(engine, assetManager, FrameBufferTextureRenderer.GAlbedoTexture, rayTraceRenderPass.ShadowTextureMask, rayTraceRenderPass.SkyboxTexture, FrameBufferTextureRenderer.GAlbedoTexture, FrameBufferTextureRenderer.GAlbedoTexture);

}

void HybridRenderer::GUIUpdate(VulkanEngine& engine)
{
    ImGui::SliderFloat3("DirectionalLight", &assetManager->SceneData->UniformDataInfo.dlight.direction.x, -1.0f, 1.0f);
    ImGui::Image(FrameBufferTextureRenderer.GPositionTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    ImGui::Image(FrameBufferTextureRenderer.GAlbedoTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    ImGui::Image(FrameBufferTextureRenderer.GNormalTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    ImGui::Image(FrameBufferTextureRenderer.GBloomTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    ImGui::Image(SSAORenderer.SSAOTexture->ImGuiDescriptorSet, ImVec2(180.0f * 4, 180.0f * 4));
    ImGui::Image(FrameBufferTextureRenderer.DepthTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
}

void HybridRenderer::Draw(VulkanEngine& engine, VulkanWindow& window, uint32_t imageIndex)
{
    FrameBufferTextureRenderer.Draw(engine, assetManager, imageIndex);
    rayTraceRenderPass.Draw(engine, assetManager, imageIndex);
    SSAORenderer.Draw(engine, assetManager, imageIndex);
    FrameBufferRenderer.Draw(engine, assetManager, imageIndex, rendererID);
}

void HybridRenderer::Destroy(VulkanEngine& engine)
{
    FrameBufferTextureRenderer.Destroy(engine);
    rayTraceRenderPass.Destroy(engine);
    FrameBufferRenderer.Destroy(engine);
}

std::vector<VkCommandBuffer> HybridRenderer::AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList)
{
    CommandBufferSubmitList.emplace_back(FrameBufferTextureRenderer.CommandBuffer);
    CommandBufferSubmitList.emplace_back(rayTraceRenderPass.RayTraceCommandBuffer);
    CommandBufferSubmitList.emplace_back(SSAORenderer.CommandBuffer);
    CommandBufferSubmitList.emplace_back(FrameBufferRenderer.CommandBuffer);
    return CommandBufferSubmitList;
}
