#include "GUIRenderer.h"

GUIRenderer::GUIRenderer() : BaseRenderer()
{
}

GUIRenderer::GUIRenderer(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window, std::shared_ptr<AssetManager> assetManagerPtr) : BaseRenderer(engine, window, assetManagerPtr)
{
    TextRenderer = GUIRenderPass(engine, assetManager);
}

GUIRenderer::~GUIRenderer()
{
}

void GUIRenderer::RebuildSwapChain(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window)
{
    TextRenderer.RebuildSwapChain(engine, assetManager);
}

void GUIRenderer::GUIUpdate(std::shared_ptr<VulkanEngine> engine)
{
    ImGui::LabelText("Mesh", "Mesh");
    for (int x = 0; x < assetManager->guiManager->GuiObjectList.size(); x++)
    {
        ImGui::SliderFloat2(("Obj Pos " + std::to_string(x)).c_str(), & assetManager->guiManager->GuiObjectList[x]->Position.x, -2.0f, 2.0f);
    }
}

void GUIRenderer::Draw(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window)
{
    TextRenderer.Draw(engine, assetManager, rendererID);
}

void GUIRenderer::Destroy(std::shared_ptr<VulkanEngine> engine)
{
    TextRenderer.Destroy(engine);
}

std::vector<VkCommandBuffer> GUIRenderer::AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList)
{
    CommandBufferSubmitList.emplace_back(TextRenderer.CommandBuffer);
    return CommandBufferSubmitList;
}