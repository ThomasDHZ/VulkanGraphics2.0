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
    for (int x = 0; x < assetManager->guiManager.GuiObjectList.size(); x++)
    {
        for (int y = 0; y < assetManager->guiManager.GuiObjectList[x]->GetGUIMeshCount(); y++)
        {
            ImGui::SliderFloat2(("Mesh Pos " + std::to_string(y)).c_str(), &assetManager->guiManager.GuiObjectList[x]->GetGUIMesh(y)->Position.x, -2.0f, 2.0f);
            ImGui::SliderFloat2(("Mesh Sacle " + std::to_string(y)).c_str(), &assetManager->guiManager.GuiObjectList[x]->GetGUIMesh(y)->Scale.x, -1.0f, 20.0f);
            ImGui::SliderFloat2(("UV offset " + std::to_string(y)).c_str(), &assetManager->guiManager.GuiObjectList[x]->GetGUIMesh(y)->UVOffset.x, -1.0f, 1.0f);
            ImGui::SliderFloat4(("Color " + std::to_string(y)).c_str(), &assetManager->guiManager.GuiObjectList[x]->GetGUIMesh(y)->Color.x, -1.0f, 1.0f);
            ImGui::LabelText("______", "______");
        }
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