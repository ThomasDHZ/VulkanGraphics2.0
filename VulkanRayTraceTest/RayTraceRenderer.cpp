#include "RayTraceRenderer.h"

RayTraceRenderer::RayTraceRenderer() : BaseRenderer()
{
}

RayTraceRenderer::RayTraceRenderer(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window, std::shared_ptr<AssetManager> assetManagerPtr) : BaseRenderer(engine, window, assetManagerPtr)
{
    rayTraceRenderPass = RayTraceRenderPass(engine, assetManager);
    FrameBufferRenderer = FrameBufferRenderPass(engine, assetManager, rayTraceRenderPass.RayTracedTexture, rayTraceRenderPass.RayTracedTexture);
}

RayTraceRenderer::~RayTraceRenderer()
{
}

void RayTraceRenderer::RebuildSwapChain(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window)
{
    rayTraceRenderPass.RebuildSwapChain(engine, assetManager, 0);
    FrameBufferRenderer.RebuildSwapChain(engine, assetManager, rayTraceRenderPass.RayTracedTexture, rayTraceRenderPass.RayTracedTexture);
}

void RayTraceRenderer::GUIUpdate(std::shared_ptr<VulkanEngine> engine)
{
    rayTraceRenderPass.GUIUpdate();

    ImGui::LabelText("Material", "Material");
    for (int x = 0; x < assetManager->materialManager->MaterialList.size(); x++)
    {
        ImGui::SliderFloat3(("Material Diffuse " + std::to_string(x)).c_str(), &assetManager->materialManager->MaterialList[x]->materialTexture.Diffuse.x, 0.0f, 1.0f);
        ImGui::SliderFloat3(("Material Specular " + std::to_string(x)).c_str(), &assetManager->materialManager->MaterialList[x]->materialTexture.Specular.x, 0.0f, 1.0f);
        //ImGui::SliderFloat3(("Material Emission " + std::to_string(x)).c_str(), &assetManager->materialManager.MaterialList[x]->materialTexture..x, 0.0f, 1.0f);
        ImGui::SliderFloat(("Material Shininess " + std::to_string(x)).c_str(), &assetManager->materialManager->MaterialList[x]->materialTexture.Shininess, 0.0f, 1.0f);
        ImGui::SliderFloat(("Material Reflectivness " + std::to_string(x)).c_str(), &assetManager->materialManager->MaterialList[x]->materialTexture.Reflectivness, 0.0f, 1.0f);
        ImGui::SliderFloat(("Material Alpha " + std::to_string(x)).c_str(), &assetManager->materialManager->MaterialList[x]->materialTexture.Alpha, 0.0f, 1.0f);
        ImGui::LabelText("______", "______");
    }

    ImGui::Image(rayTraceRenderPass.RayTracedTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    ImGui::LabelText("Directional Light", "Directional Light");
    for (int x = 0; x < assetManager->lightManager->DirectionalLightList.size(); x++)
    {
        ImGui::SliderFloat3(("DLight direction " + std::to_string(x)).c_str(), &assetManager->lightManager->DirectionalLightList[x]->LightBuffer.UniformDataInfo.direction.x, -1.0f, 1.0f);
        ImGui::SliderFloat3(("DLight ambient " + std::to_string(x)).c_str(), &assetManager->lightManager->DirectionalLightList[x]->LightBuffer.UniformDataInfo.ambient.x, 0.0f, 1.0f);
        ImGui::SliderFloat3(("DLight Diffuse " + std::to_string(x)).c_str(), &assetManager->lightManager->DirectionalLightList[x]->LightBuffer.UniformDataInfo.diffuse.x, 0.0f, 1.0f);
        ImGui::SliderFloat3(("DLight specular " + std::to_string(x)).c_str(), &assetManager->lightManager->DirectionalLightList[x]->LightBuffer.UniformDataInfo.specular.x, 0.0f, 1.0f);
        ImGui::LabelText("______", "______");
    }

    ImGui::LabelText("Point Light", "Point Light");
    for (int x = 0; x < assetManager->lightManager->PointLightList.size(); x++)
    {
        ImGui::SliderFloat3(("PLight position " + std::to_string(x)).c_str(), &assetManager->lightManager->PointLightList[x]->LightBuffer.UniformDataInfo.position.x, -100.0f, 100.0f);
        ImGui::SliderFloat3(("PLight ambient " + std::to_string(x)).c_str(), &assetManager->lightManager->PointLightList[x]->LightBuffer.UniformDataInfo.ambient.x, 0.0f, 1.0f);
        ImGui::SliderFloat3(("PLight Diffuse " + std::to_string(x)).c_str(), &assetManager->lightManager->PointLightList[x]->LightBuffer.UniformDataInfo.diffuse.x, 0.0f, 1.0f);
        ImGui::SliderFloat3(("PLight specular " + std::to_string(x)).c_str(), &assetManager->lightManager->PointLightList[x]->LightBuffer.UniformDataInfo.specular.x, 0.0f, 1.0f);
        ImGui::SliderFloat(("PLight constant " + std::to_string(x)).c_str(), &assetManager->lightManager->PointLightList[x]->LightBuffer.UniformDataInfo.constant, 0.0f, 1.0f);
        ImGui::SliderFloat(("PLight linear " + std::to_string(x)).c_str(), &assetManager->lightManager->PointLightList[x]->LightBuffer.UniformDataInfo.linear, 0.0f, 1.0f);
        ImGui::SliderFloat(("PLight quadratic " + std::to_string(x)).c_str(), &assetManager->lightManager->PointLightList[x]->LightBuffer.UniformDataInfo.quadratic, 0.0f, 1.0f);
        ImGui::LabelText("______", "______");
    }

    ImGui::LabelText("SpotLight Light", "Directional Light");
    for (int x = 0; x < assetManager->lightManager->SpotLightList.size(); x++)
    {
        ImGui::SliderFloat3(("SLight position " + std::to_string(x)).c_str(), &assetManager->lightManager->SpotLightList[x]->LightBuffer.UniformDataInfo.position.x, -10.0f, 10.0f);
        ImGui::SliderFloat3(("SLight direction " + std::to_string(x)).c_str(), &assetManager->lightManager->SpotLightList[x]->LightBuffer.UniformDataInfo.direction.x, -1.0f, 1.0f);
        ImGui::SliderFloat3(("SLight ambient " + std::to_string(x)).c_str(), &assetManager->lightManager->SpotLightList[x]->LightBuffer.UniformDataInfo.ambient.x, 0.0f, 1.0f);
        ImGui::SliderFloat3(("SLight Diffuse " + std::to_string(x)).c_str(), &assetManager->lightManager->SpotLightList[x]->LightBuffer.UniformDataInfo.diffuse.x, 0.0f, 1.0f);
        ImGui::SliderFloat3(("SLight specular " + std::to_string(x)).c_str(), &assetManager->lightManager->SpotLightList[x]->LightBuffer.UniformDataInfo.specular.x, 0.0f, 1.0f);
        ImGui::SliderFloat(("SLight constant " + std::to_string(x)).c_str(), &assetManager->lightManager->SpotLightList[x]->LightBuffer.UniformDataInfo.constant, 0.0f, 1.0f);
        ImGui::SliderFloat(("SLight linear " + std::to_string(x)).c_str(), &assetManager->lightManager->SpotLightList[x]->LightBuffer.UniformDataInfo.linear, 0.0f, 1.0f);
        ImGui::SliderFloat(("SLight quadratic " + std::to_string(x)).c_str(), &assetManager->lightManager->SpotLightList[x]->LightBuffer.UniformDataInfo.quadratic, 0.0f, 1.0f);
        ImGui::LabelText("______", "______");
    }
}

void RayTraceRenderer::Draw(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window, uint32_t imageIndex)
{
    rayTraceRenderPass.Draw(engine, assetManager, imageIndex, rendererID, assetManager->cameraManager->ActiveCamera);
      FrameBufferRenderer.Draw(engine, assetManager, imageIndex);
}

void RayTraceRenderer::Destroy(std::shared_ptr<VulkanEngine> engine)
{
    rayTraceRenderPass.Destroy(engine);
      FrameBufferRenderer.Destroy(engine);
}

std::vector<VkCommandBuffer> RayTraceRenderer::AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList)
{
    CommandBufferSubmitList.emplace_back(rayTraceRenderPass.RayTraceCommandBuffer);
    CommandBufferSubmitList.emplace_back(FrameBufferRenderer.CommandBuffer);
    return CommandBufferSubmitList;
}
