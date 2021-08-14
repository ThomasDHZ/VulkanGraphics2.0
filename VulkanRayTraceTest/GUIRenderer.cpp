#include "GUIRenderer.h"

GUIRenderer::GUIRenderer() : BaseRenderer()
{
}

GUIRenderer::GUIRenderer(std::shared_ptr<VulkanEngine> engine) : BaseRenderer()
{
    TextRenderer = GUIRenderPass(engine);
}

GUIRenderer::~GUIRenderer()
{
}

void GUIRenderer::RebuildSwapChain()
{
    TextRenderer.RebuildSwapChain();
}

void GUIRenderer::GUIUpdate()
{
    ImGui::LabelText("Mesh", "Mesh");
    for (int x = 0; x < AssetManagerPtr::GetAssetPtr()->guiManager->GuiObjectList.size(); x++)
    {
        ImGui::SliderFloat2(("Obj Pos " + std::to_string(x)).c_str(), &AssetManagerPtr::GetAssetPtr()->guiManager->GuiObjectList[x]->Position.x, -2.0f, 2.0f);
    }
}

void GUIRenderer::Draw()
{
    TextRenderer.Draw();
}

void GUIRenderer::Destroy()
{
    TextRenderer.Destroy();
}

std::vector<VkCommandBuffer> GUIRenderer::AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList)
{
    CommandBufferSubmitList.emplace_back(TextRenderer.CommandBuffer);
    return CommandBufferSubmitList;
}