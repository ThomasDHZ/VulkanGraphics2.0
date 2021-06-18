#include "Renderer2D.h"

Renderer2D::Renderer2D() : BaseRenderer()
{
}

Renderer2D::Renderer2D(VulkanEngine& engine, std::shared_ptr<VulkanWindow> window, std::shared_ptr<AssetManager> assetManagerPtr) : BaseRenderer(engine, window, assetManagerPtr)
{
    FrameBufferTextureRenderer = RenderPass2D(engine, assetManager);
    BloomRenderer = BloomRenderPass(engine, assetManager, FrameBufferTextureRenderer.BloomTexture);
    FrameBufferRenderer = FrameBufferRenderPass(engine, assetManager, FrameBufferTextureRenderer.RenderedTexture, BloomRenderer.BloomTexture);
}

Renderer2D::~Renderer2D()
{
}

void Renderer2D::RebuildSwapChain(VulkanEngine& engine, std::shared_ptr<VulkanWindow> window)
{
    FrameBufferTextureRenderer.RebuildSwapChain(engine, assetManager);
    BloomRenderer.RebuildSwapChain(engine, assetManager, FrameBufferTextureRenderer.BloomTexture);
    FrameBufferRenderer.RebuildSwapChain(engine, assetManager, FrameBufferTextureRenderer.RenderedTexture, BloomRenderer.BloomTexture);
}

void Renderer2D::GUIUpdate(VulkanEngine& engine)
{
    for (int x = 0; x < assetManager->meshManager.MeshList.size(); x++)
    {
        ImGui::SliderFloat3(("Mesh Pos " + std::to_string(x)).c_str(), &assetManager->meshManager.MeshList[x]->MeshPosition.x, -100.0f, 100.0f);
        ImGui::SliderFloat2(("UV " + std::to_string(x)).c_str(), &assetManager->meshManager.MeshList[x]->MeshProperties.UniformDataInfo.UVOffset.x, 0.0f, 1.0f);
        ImGui::SliderFloat2(("Flip UV " + std::to_string(x)).c_str(), &assetManager->meshManager.MeshList[x]->MeshProperties.UniformDataInfo.UVFlip.x, 0.0f, 1.0f);
        ImGui::LabelText("______", "______");
    }
}

void Renderer2D::Draw(VulkanEngine& engine, std::shared_ptr<VulkanWindow> window, uint32_t imageIndex)
{
    FrameBufferTextureRenderer.Draw(engine, assetManager, imageIndex, rendererID);
    BloomRenderer.Draw(engine, assetManager, imageIndex);
    FrameBufferRenderer.Draw(engine, assetManager, imageIndex);
}

void Renderer2D::Destroy(VulkanEngine& engine)
{
    FrameBufferTextureRenderer.Destroy(engine);
    BloomRenderer.Destroy(engine);
    FrameBufferRenderer.Destroy(engine);
}

std::vector<VkCommandBuffer> Renderer2D::AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList)
{
    CommandBufferSubmitList.emplace_back(FrameBufferTextureRenderer.CommandBuffer);
    CommandBufferSubmitList.emplace_back(BloomRenderer.CommandBuffer);
    CommandBufferSubmitList.emplace_back(FrameBufferRenderer.CommandBuffer);
    return CommandBufferSubmitList;
}