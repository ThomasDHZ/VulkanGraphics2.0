#include "GUIRenderer.h"

GUIRenderer::GUIRenderer() : BaseRenderer()
{
}

GUIRenderer::GUIRenderer(VulkanEngine& engine, std::shared_ptr<VulkanWindow> window, std::shared_ptr<AssetManager> assetManagerPtr) : BaseRenderer(engine, window, assetManagerPtr)
{
    TextRenderer = GUIRenderPass(engine, assetManager);
}

GUIRenderer::~GUIRenderer()
{
}

void GUIRenderer::RebuildSwapChain(VulkanEngine& engine, std::shared_ptr<VulkanWindow> window)
{
    TextRenderer.RebuildSwapChain(engine, assetManager);
}

void GUIRenderer::GUIUpdate(VulkanEngine& engine)
{
    ImGui::LabelText("Mesh", "Mesh");
    for (int x = 0; x < assetManager->meshManager.GUIMeshList.size(); x++)
    {
        ImGui::SliderFloat2(("Mesh Pos " + std::to_string(x)).c_str(), &assetManager->meshManager.GUIMeshList[x]->Translation.x, -1.0f, 1.0f);
        ImGui::SliderFloat2(("Mesh Rot " + std::to_string(x)).c_str(), &assetManager->meshManager.GUIMeshList[x]->Scale.x, -1.0f, 1.0f);
        ImGui::SliderFloat4(("Color " + std::to_string(x)).c_str(), &assetManager->meshManager.GUIMeshList[x]->Color.x, -1.0f, 1.0f);
        ImGui::LabelText("______", "______");
    }
}

void GUIRenderer::Draw(VulkanEngine& engine, std::shared_ptr<VulkanWindow> window, uint32_t imageIndex)
{
    TextRenderer.Draw(engine, assetManager, imageIndex, rendererID);
}

void GUIRenderer::Destroy(VulkanEngine& engine)
{
    TextRenderer.Destroy(engine);
}

std::vector<VkCommandBuffer> GUIRenderer::AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList)
{
    CommandBufferSubmitList.emplace_back(TextRenderer.CommandBuffer);
    return CommandBufferSubmitList;
}