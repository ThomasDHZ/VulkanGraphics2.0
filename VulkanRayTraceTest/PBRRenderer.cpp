#include "PBRRenderer.h"

PBRRenderer::PBRRenderer() : BaseRenderer()
{
}

PBRRenderer::PBRRenderer(VulkanEngine& engine, VulkanWindow& window, std::shared_ptr<AssetManager> assetManagerPtr) : BaseRenderer(engine, window, assetManagerPtr)
{
    cubeMapRenderer = CubeMapRenderPass(engine, assetManager, 512.0f);
    prefilterRenderPass = PrefilterRenderPass(engine, assetManager, 512.0f);
    brdfRenderPass = BRDFRenderPass(engine, assetManager, cubeMapRenderer.BlurredSkyBoxTexture);
    FrameBufferTextureRenderer = FrameBufferTextureRenderPass(engine, assetManager, cubeMapRenderer.BlurredSkyBoxTexture, prefilterRenderPass.BlurredSkyBoxTexture, brdfRenderPass.BloomTexture);
    DebugDepthRenderer = DepthDebugRenderPass(engine, assetManager, FrameBufferTextureRenderer.DepthTexture);
    FrameBufferRenderer = FrameBufferRenderPass(engine, assetManager, FrameBufferTextureRenderer.RenderedTexture, FrameBufferTextureRenderer.BloomTexture);


}

PBRRenderer::~PBRRenderer()
{
}

void PBRRenderer::RebuildSwapChain(VulkanEngine& engine, VulkanWindow& window)
{
    cubeMapRenderer.RebuildSwapChain(engine);
    prefilterRenderPass.RebuildSwapChain(engine);
    brdfRenderPass.RebuildSwapChain(engine, assetManager, cubeMapRenderer.BlurredSkyBoxTexture);
    FrameBufferTextureRenderer.RebuildSwapChain(engine, assetManager);
    DebugDepthRenderer.RebuildSwapChain(engine, assetManager, FrameBufferTextureRenderer.DepthTexture);
    FrameBufferRenderer.RebuildSwapChain(engine, assetManager, FrameBufferTextureRenderer.RenderedTexture, FrameBufferTextureRenderer.BloomTexture);
}

void PBRRenderer::GUIUpdate(VulkanEngine& engine)
{
    ImGui::SliderFloat3("DirectionalLight", &assetManager->SceneData->UniformDataInfo.dlight.direction.x, -1.0f, 1.0f);
    ImGui::Image(FrameBufferTextureRenderer.RenderedTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    //  ImGui::Image(FrameBufferTextureRenderer.BloomTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    ImGui::Image(DebugDepthRenderer.DebugDepthTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    ImGui::Image(cubeMapRenderer.RenderedTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    ImGui::Image(prefilterRenderPass.RenderedTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    ImGui::Image(brdfRenderPass.BloomTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
}

void PBRRenderer::Draw(VulkanEngine& engine, VulkanWindow& window, uint32_t imageIndex)
{
    //    cubeMapRenderer.Draw(engine, assetManager, imageIndex);
    //prefilterRenderPass.Draw(engine, assetManager, imageIndex);
    // brdfRenderPass.Draw(engine, assetManager, imageIndex);
    FrameBufferTextureRenderer.Draw(engine, assetManager, imageIndex, rendererID);
    DebugDepthRenderer.Draw(engine, assetManager, imageIndex);
    FrameBufferRenderer.Draw(engine, assetManager, imageIndex);
}

void PBRRenderer::Destroy(VulkanEngine& engine)
{
    FrameBufferTextureRenderer.Destroy(engine);
    DebugDepthRenderer.Destroy(engine);
    FrameBufferRenderer.Destroy(engine);
    cubeMapRenderer.Destroy(engine);
    prefilterRenderPass.Destroy(engine);
    brdfRenderPass.Destroy(engine);
}

std::vector<VkCommandBuffer> PBRRenderer::AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList)
{
    //    CommandBufferSubmitList.emplace_back(cubeMapRenderer.CommandBuffer);
    //CommandBufferSubmitList.emplace_back(prefilterRenderPass.CommandBuffer);
    // CommandBufferSubmitList.emplace_back(brdfRenderPass.CommandBuffer);
    CommandBufferSubmitList.emplace_back(FrameBufferTextureRenderer.CommandBuffer);
    CommandBufferSubmitList.emplace_back(DebugDepthRenderer.CommandBuffer);
    CommandBufferSubmitList.emplace_back(FrameBufferRenderer.CommandBuffer);
    return CommandBufferSubmitList;
}
