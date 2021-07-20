#include "GUIRenderer.h"

GUIRenderer::GUIRenderer() : BaseRenderer()
{
}

GUIRenderer::GUIRenderer(VulkanEngine& engine, std::shared_ptr<VulkanWindow> window, std::shared_ptr<AssetManager> assetManagerPtr) : BaseRenderer(engine, window, assetManagerPtr)
{
    TextRenderer = TextRenderPass(engine, assetManager);
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
    for (int x = 0; x < assetManager->meshManager.MeshList.size(); x++)
    {
        ImGui::SliderFloat3(("Mesh Pos " + std::to_string(x)).c_str(), &assetManager->meshManager.MeshList[x]->MeshPosition.x, -100.0f, 100.0f);
        ImGui::SliderFloat3(("Mesh Rot " + std::to_string(x)).c_str(), &assetManager->meshManager.MeshList[x]->MeshRotation.x, -360.0f, 360.0f);
        ImGui::SliderFloat2(("UV " + std::to_string(x)).c_str(), &assetManager->meshManager.MeshList[x]->MeshProperties.UniformDataInfo.UVOffset.x, 0.0f, 1.0f);
        ImGui::SliderFloat2(("UV Scale " + std::to_string(x)).c_str(), &assetManager->meshManager.MeshList[x]->UVScale.x, 0.0f, 2.0f);
        ImGui::SliderFloat2(("UV Flip " + std::to_string(x)).c_str(), &assetManager->meshManager.MeshList[x]->UVFlip.x, 0.0f, 1.0f);
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