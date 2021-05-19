#include "BlinnPhongRasterRenderer.h"
#include "BillboardMesh.h"

BlinnPhongRasterRenderer::BlinnPhongRasterRenderer() : BaseRenderer()
{
}

BlinnPhongRasterRenderer::BlinnPhongRasterRenderer(VulkanEngine& engine, VulkanWindow& window, std::shared_ptr<AssetManager> assetManagerPtr) : BaseRenderer(engine, window, assetManagerPtr)
{
    FrameBufferTextureRenderer = FrameBufferTextureRenderPass(engine, assetManager);
    BloomRenderer = BloomRenderPass(engine, assetManager, FrameBufferTextureRenderer.BloomTexture);
    DebugDepthRenderer = DepthDebugRenderPass(engine, assetManager, FrameBufferTextureRenderer.DepthTexture);
    FrameBufferRenderer = FrameBufferRenderPass(engine, assetManager, FrameBufferTextureRenderer.RenderedTexture, BloomRenderer.BloomTexture);
}

BlinnPhongRasterRenderer::~BlinnPhongRasterRenderer()
{
}

void BlinnPhongRasterRenderer::RebuildSwapChain(VulkanEngine& engine, VulkanWindow& window)
{
    FrameBufferTextureRenderer.RebuildSwapChain(engine, assetManager);
    BloomRenderer.RebuildSwapChain(engine, assetManager, FrameBufferTextureRenderer.BloomTexture);
    DebugDepthRenderer.RebuildSwapChain(engine, assetManager, FrameBufferTextureRenderer.DepthTexture);
    FrameBufferRenderer.RebuildSwapChain(engine, assetManager, FrameBufferTextureRenderer.RenderedTexture, BloomRenderer.BloomTexture);
}

void BlinnPhongRasterRenderer::GUIUpdate(VulkanEngine& engine)
{
    ImGui::SliderFloat3("dlight", &assetManager->lightManager.DirectionalLightList[0]->LightBuffer.UniformDataInfo.ambient.x, 0.0f, 1.0f);
    ImGui::SliderFloat3("dlight2", &assetManager->lightManager.DirectionalLightList[1]->LightBuffer.UniformDataInfo.ambient.x, 0.0f, 1.0f);

    ImGui::SliderFloat3("a", &assetManager->meshManager.MeshList[1]->MeshRotation.x, 0.0f, 180.0f);
    ImGui::SliderInt("Frame", &static_cast<BillboardMesh*>(assetManager->meshManager.MeshList[1].get())->animator.CurrentFrame, 0, 60);
    ImGui::Image(FrameBufferTextureRenderer.RenderedTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    ImGui::Image(FrameBufferTextureRenderer.BloomTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    ImGui::Image(DebugDepthRenderer.DebugDepthTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    ImGui::Image(BloomRenderer.BloomTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
}

void BlinnPhongRasterRenderer::Draw(VulkanEngine& engine, VulkanWindow& window, uint32_t imageIndex)
{
    FrameBufferTextureRenderer.Draw(engine, assetManager, imageIndex, rendererID);
    BloomRenderer.Draw(engine, assetManager, imageIndex);
    DebugDepthRenderer.Draw(engine, assetManager, imageIndex);
    FrameBufferRenderer.Draw(engine, assetManager, imageIndex);
}

void BlinnPhongRasterRenderer::Destroy(VulkanEngine& engine)
{
    FrameBufferTextureRenderer.Destroy(engine);
    BloomRenderer.Destroy(engine);
    DebugDepthRenderer.Destroy(engine);
    FrameBufferRenderer.Destroy(engine);;
}

std::vector<VkCommandBuffer> BlinnPhongRasterRenderer::AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList)
{
    CommandBufferSubmitList.emplace_back(FrameBufferTextureRenderer.CommandBuffer);
    CommandBufferSubmitList.emplace_back(BloomRenderer.CommandBuffer);
    CommandBufferSubmitList.emplace_back(DebugDepthRenderer.CommandBuffer);
    CommandBufferSubmitList.emplace_back(FrameBufferRenderer.CommandBuffer);
    return CommandBufferSubmitList;
}