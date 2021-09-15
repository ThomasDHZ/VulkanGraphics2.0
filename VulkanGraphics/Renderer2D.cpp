#include "Renderer2D.h"

Renderer2D::Renderer2D() : BaseRenderer()
{
}

Renderer2D::Renderer2D(std::shared_ptr<VulkanEngine> engine) : BaseRenderer()
{
    rendererPass2D = RenderPass2D(engine);
    FrameBufferRenderer = FrameBufferRenderPass(rendererPass2D.RenderedTexture, rendererPass2D.RenderedTexture);
}

Renderer2D::~Renderer2D()
{
}

void Renderer2D::RebuildSwapChain()
{
    rendererPass2D.RebuildSwapChain();
    FrameBufferRenderer.RebuildSwapChain(rendererPass2D.RenderedTexture, rendererPass2D.BloomTexture);
}

void Renderer2D::GUIUpdate()
{
    ImGui::Checkbox("Mesh Debug: ", &rendererPass2D.WireFrameFlag);
    ImGui::LabelText("Mesh", "Mesh");
    for (int x = 0; x < AssetManagerPtr::GetAssetPtr()->meshManager->MeshList.size(); x++)
    {
        ImGui::SliderFloat3(("Mesh Pos " + std::to_string(x)).c_str(), &AssetManagerPtr::GetAssetPtr()->meshManager->MeshList[x]->MeshPosition.x, -100.0f, 100.0f);
        ImGui::SliderFloat2(("UV " + std::to_string(x)).c_str(), &AssetManagerPtr::GetAssetPtr()->meshManager->MeshList[x]->MeshProperties.UniformDataInfo.UVOffset.x, 0.0f, 1.0f);
        ImGui::SliderFloat2(("UV Scale " + std::to_string(x)).c_str(), &AssetManagerPtr::GetAssetPtr()->meshManager->MeshList[x]->UVScale.x, 0.0f, 2.0f);
        ImGui::SliderFloat2(("UV Flip " + std::to_string(x)).c_str(), &AssetManagerPtr::GetAssetPtr()->meshManager->MeshList[x]->UVFlip.x, 0.0f, 1.0f);
        ImGui::LabelText("______", "______");
    }
}

void Renderer2D::Draw()
{
    rendererPass2D.Draw();
    FrameBufferRenderer.Draw();
}

void Renderer2D::Destroy()
{
    rendererPass2D.Destroy();
    FrameBufferRenderer.Destroy();
}

std::vector<VkCommandBuffer> Renderer2D::AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList)
{
    CommandBufferSubmitList.emplace_back(rendererPass2D.GetCommandBuffer());
    CommandBufferSubmitList.emplace_back(FrameBufferRenderer.GetCommandBuffer());
    return CommandBufferSubmitList;
}