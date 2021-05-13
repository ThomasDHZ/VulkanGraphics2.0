#include "HybridRenderer.h"

HybridRenderer::HybridRenderer() : BaseRenderer()
{
}

HybridRenderer::HybridRenderer(VulkanEngine& engine, VulkanWindow& window, std::shared_ptr<AssetManager> assetManagerPtr) : BaseRenderer(engine, window, assetManagerPtr)
{
    FrameBufferTextureRenderer = GBufferRenderPass(engine, assetManager);
    rayTraceRenderPass = RayTraceRenderPass(engine, assetManager);
    bloomRenderPass = BloomRenderPass(engine, assetManager, FrameBufferTextureRenderer.GBloomTexture);

    SSAOTextureList textures = {};
    textures.GPositionTexture = FrameBufferTextureRenderer.GPositionTexture;
    textures.GNormalTexture = FrameBufferTextureRenderer.GNormalTexture;
    SSAORenderer = SSAORenderPass(engine, assetManager, textures);
    SSAOBlurRenderer = SSAOBlurRenderPass(engine, assetManager, SSAORenderer.SSAOTexture);


    HybridFrameBufferTextures frameBufferTextures{};
    frameBufferTextures.AlebdoTexture = FrameBufferTextureRenderer.GAlbedoTexture;
    frameBufferTextures.PositionTexture = FrameBufferTextureRenderer.GPositionTexture;
    frameBufferTextures.NormalTexture = FrameBufferTextureRenderer.GNormalTexture;
    frameBufferTextures.BloomTexture = bloomRenderPass.BloomTexture;
    frameBufferTextures.SSA0Texture = SSAOBlurRenderer.SSAOBlurTexture;
    frameBufferTextures.ShadowTexture = rayTraceRenderPass.ShadowTextureMask;
    frameBufferTextures.ReflectionTexture = rayTraceRenderPass.ReflectionTexture;
    frameBufferTextures.SkyBoxTexture = rayTraceRenderPass.SkyboxTexture;
    FrameBufferRenderer = HybridFrameBufferRenderPass(engine, assetManager, frameBufferTextures);

    CurrentSSAOSampleRate = SSAORenderer.KernalSampleSize;
}

HybridRenderer::~HybridRenderer()
{
}

void HybridRenderer::RebuildSwapChain(VulkanEngine& engine, VulkanWindow& window)
{
    FrameBufferTextureRenderer.RebuildSwapChain(engine, assetManager);
    rayTraceRenderPass.RebuildSwapChain(engine, assetManager, 0);
    bloomRenderPass.RebuildSwapChain(engine, assetManager, FrameBufferTextureRenderer.GBloomTexture);

    SSAOTextureList textures = {};
    textures.GPositionTexture = FrameBufferTextureRenderer.GPositionTexture;
    textures.GNormalTexture = FrameBufferTextureRenderer.GNormalTexture;
    SSAORenderer = SSAORenderPass(engine, assetManager, textures);
    SSAOBlurRenderer = SSAOBlurRenderPass(engine, assetManager, SSAORenderer.SSAOTexture);

    HybridFrameBufferTextures frameBufferTextures{};
    frameBufferTextures.AlebdoTexture = FrameBufferTextureRenderer.GAlbedoTexture;
    frameBufferTextures.PositionTexture = FrameBufferTextureRenderer.GPositionTexture;
    frameBufferTextures.NormalTexture = FrameBufferTextureRenderer.GNormalTexture;
    frameBufferTextures.BloomTexture = bloomRenderPass.BloomTexture;
    frameBufferTextures.SSA0Texture = SSAOBlurRenderer.SSAOBlurTexture;
    frameBufferTextures.ShadowTexture = rayTraceRenderPass.ShadowTextureMask;
    frameBufferTextures.ReflectionTexture = rayTraceRenderPass.ReflectionTexture;
    frameBufferTextures.SkyBoxTexture = rayTraceRenderPass.SkyboxTexture;
    FrameBufferRenderer.RebuildSwapChain(engine, assetManager, frameBufferTextures);

}

void HybridRenderer::GUIUpdate(VulkanEngine& engine)
{
    ImGui::Checkbox("Apply SSAO", &ApplySSAO);
    ImGui::SliderInt("Apply SSAO", &assetManager->SceneData->UniformDataInfo.temp, 0, 1);
    ImGui::SliderFloat3("DirectionalLight", &assetManager->SceneData->UniformDataInfo.dlight.direction.x, -1.0f, 1.0f);
    ImGui::SliderInt("SSAOSample", &SSAORenderer.KernalSampleSize, 0, 2550);
    ImGui::SliderFloat("SSAOBias", &SSAORenderer.bias, 0.0f, 100.0f);
    ImGui::SliderFloat("SSAORadius", &SSAORenderer.radius, 0.0f, 10000000.0f);
    ImGui::Image(FrameBufferTextureRenderer.GPositionTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    ImGui::Image(FrameBufferTextureRenderer.GAlbedoTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    ImGui::Image(FrameBufferTextureRenderer.GNormalTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    ImGui::Image(FrameBufferTextureRenderer.GBloomTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    ImGui::Image(bloomRenderPass.BloomTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    if (ApplySSAO)
    {
        ImGui::Image(SSAORenderer.SSAOTexture->ImGuiDescriptorSet, ImVec2(180.0f * 4, 180.0f * 4));
        ImGui::Image(SSAOBlurRenderer.SSAOBlurTexture->ImGuiDescriptorSet, ImVec2(180.0f * 4, 180.0f * 4));
    }
    ImGui::Image(FrameBufferTextureRenderer.DepthTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));

    if (CurrentSSAOSampleRate != SSAORenderer.KernalSampleSize)
    {
        SSAOTextureList textures = {};
        textures.GPositionTexture = FrameBufferTextureRenderer.GPositionTexture;
        textures.GNormalTexture = FrameBufferTextureRenderer.GNormalTexture;
        SSAORenderer.RebuildSwapChain(engine, assetManager, textures);

        CurrentSSAOSampleRate = SSAORenderer.KernalSampleSize;
    }
}

void HybridRenderer::Draw(VulkanEngine& engine, VulkanWindow& window, uint32_t imageIndex)
{
    FrameBufferTextureRenderer.Draw(engine, assetManager, imageIndex);
    rayTraceRenderPass.Draw(engine, assetManager, imageIndex);
    bloomRenderPass.Draw(engine, assetManager, imageIndex);
    if (ApplySSAO)
    {
        SSAORenderer.Draw(engine, assetManager, imageIndex);
        SSAOBlurRenderer.Draw(engine, assetManager, imageIndex);
    }
    FrameBufferRenderer.Draw(engine, assetManager, imageIndex, rendererID);
}

void HybridRenderer::Destroy(VulkanEngine& engine)
{
    FrameBufferTextureRenderer.Destroy(engine);
    rayTraceRenderPass.Destroy(engine);
    bloomRenderPass.Destroy(engine);
    SSAORenderer.Destroy(engine);
    SSAOBlurRenderer.Destroy(engine);
    FrameBufferRenderer.Destroy(engine);
}

std::vector<VkCommandBuffer> HybridRenderer::AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList)
{
    CommandBufferSubmitList.emplace_back(FrameBufferTextureRenderer.CommandBuffer);
    CommandBufferSubmitList.emplace_back(rayTraceRenderPass.RayTraceCommandBuffer);
    CommandBufferSubmitList.emplace_back(bloomRenderPass.CommandBuffer);
    if (ApplySSAO)
    {
        CommandBufferSubmitList.emplace_back(SSAORenderer.CommandBuffer);
        CommandBufferSubmitList.emplace_back(SSAOBlurRenderer.CommandBuffer);
    }
    CommandBufferSubmitList.emplace_back(FrameBufferRenderer.CommandBuffer);
    return CommandBufferSubmitList;
}
