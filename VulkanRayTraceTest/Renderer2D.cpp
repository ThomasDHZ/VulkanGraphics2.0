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

    ImGui::LabelText("Directional Light", "Directional Light");
    for (int x = 0; x < assetManager->lightManager.DirectionalLightList.size(); x++)
    {
        ImGui::SliderFloat3(("DLight direction " + std::to_string(x)).c_str(), &assetManager->lightManager.DirectionalLightList[x]->LightBuffer.UniformDataInfo.direction.x, -1.0f, 1.0f);
        ImGui::SliderFloat3(("DLight ambient " + std::to_string(x)).c_str(), &assetManager->lightManager.DirectionalLightList[x]->LightBuffer.UniformDataInfo.ambient.x, 0.0f, 1.0f);
        ImGui::SliderFloat3(("DLight Diffuse " + std::to_string(x)).c_str(), &assetManager->lightManager.DirectionalLightList[x]->LightBuffer.UniformDataInfo.diffuse.x, 0.0f, 1.0f);
        ImGui::SliderFloat3(("DLight specular " + std::to_string(x)).c_str(), &assetManager->lightManager.DirectionalLightList[x]->LightBuffer.UniformDataInfo.specular.x, 0.0f, 1.0f);
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