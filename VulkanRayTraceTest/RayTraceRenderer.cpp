#include "RayTraceRenderer.h"

RayTraceRenderer::RayTraceRenderer() : BaseRenderer()
{
}

RayTraceRenderer::RayTraceRenderer(std::shared_ptr<VulkanEngine> engine) : BaseRenderer()
{
    rayTraceRenderPass = RayTraceRenderPass(engine, AssetManagerPtr::GetAssetPtr());
   // FrameBufferRenderer = FrameBufferRenderPass(rayTraceRenderPass.RayTracedTexture, rayTraceRenderPass.RayTracedTexture);
}

RayTraceRenderer::~RayTraceRenderer()
{
}

void RayTraceRenderer::RebuildSwapChain()
{
    rayTraceRenderPass.RebuildSwapChain(EnginePtr::GetEnginePtr(), AssetManagerPtr::GetAssetPtr());
    FrameBufferRenderer.RebuildSwapChain(rayTraceRenderPass.RayTracedTexture, rayTraceRenderPass.RayTracedTexture);
}

void RayTraceRenderer::GUIUpdate()
{
    rayTraceRenderPass.GUIUpdate();

    ImGui::LabelText("Material", "Material");
    for (int x = 0; x < AssetManagerPtr::GetAssetPtr()->materialManager->MaterialList.size(); x++)
    {
        ImGui::SliderFloat3(("Material Diffuse " + std::to_string(x)).c_str(), &AssetManagerPtr::GetAssetPtr()->materialManager->MaterialList[x]->materialTexture.Diffuse.x, 0.0f, 1.0f);
        ImGui::SliderFloat3(("Material Specular " + std::to_string(x)).c_str(), &AssetManagerPtr::GetAssetPtr()->materialManager->MaterialList[x]->materialTexture.Specular.x, 0.0f, 1.0f);
        //ImGui::SliderFloat3(("Material Emission " + std::to_string(x)).c_str(), &AssetManagerPtr::GetAssetPtr()->materialManager.MaterialList[x]->materialTexture..x, 0.0f, 1.0f);
        ImGui::SliderFloat(("Material Shininess " + std::to_string(x)).c_str(), &AssetManagerPtr::GetAssetPtr()->materialManager->MaterialList[x]->materialTexture.Shininess, 0.0f, 1.0f);
        ImGui::SliderFloat(("Material Reflectivness " + std::to_string(x)).c_str(), &AssetManagerPtr::GetAssetPtr()->materialManager->MaterialList[x]->materialTexture.Reflectivness, 0.0f, 1.0f);
        ImGui::SliderFloat(("Material Alpha " + std::to_string(x)).c_str(), &AssetManagerPtr::GetAssetPtr()->materialManager->MaterialList[x]->materialTexture.Alpha, 0.0f, 1.0f);
        ImGui::LabelText("______", "______");
    }

    ImGui::Image(rayTraceRenderPass.RayTracedTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    ImGui::LabelText("Directional Light", "Directional Light");
    for (int x = 0; x < AssetManagerPtr::GetAssetPtr()->lightManager->DirectionalLightList.size(); x++)
    {
        ImGui::SliderFloat3(("DLight direction " + std::to_string(x)).c_str(), &AssetManagerPtr::GetAssetPtr()->lightManager->DirectionalLightList[x]->LightBuffer.UniformDataInfo.direction.x, -1.0f, 1.0f);
        ImGui::SliderFloat3(("DLight ambient " + std::to_string(x)).c_str(), &AssetManagerPtr::GetAssetPtr()->lightManager->DirectionalLightList[x]->LightBuffer.UniformDataInfo.ambient.x, 0.0f, 1.0f);
        ImGui::SliderFloat3(("DLight Diffuse " + std::to_string(x)).c_str(), &AssetManagerPtr::GetAssetPtr()->lightManager->DirectionalLightList[x]->LightBuffer.UniformDataInfo.diffuse.x, 0.0f, 1.0f);
        ImGui::SliderFloat3(("DLight specular " + std::to_string(x)).c_str(), &AssetManagerPtr::GetAssetPtr()->lightManager->DirectionalLightList[x]->LightBuffer.UniformDataInfo.specular.x, 0.0f, 1.0f);
        ImGui::LabelText("______", "______");
    }

    ImGui::LabelText("Point Light", "Point Light");
    for (int x = 0; x < AssetManagerPtr::GetAssetPtr()->lightManager->PointLightList.size(); x++)
    {
        ImGui::SliderFloat3(("PLight position " + std::to_string(x)).c_str(), &AssetManagerPtr::GetAssetPtr()->lightManager->PointLightList[x]->LightBuffer.UniformDataInfo.position.x, -100.0f, 100.0f);
        ImGui::SliderFloat3(("PLight ambient " + std::to_string(x)).c_str(), &AssetManagerPtr::GetAssetPtr()->lightManager->PointLightList[x]->LightBuffer.UniformDataInfo.ambient.x, 0.0f, 1.0f);
        ImGui::SliderFloat3(("PLight Diffuse " + std::to_string(x)).c_str(), &AssetManagerPtr::GetAssetPtr()->lightManager->PointLightList[x]->LightBuffer.UniformDataInfo.diffuse.x, 0.0f, 1.0f);
        ImGui::SliderFloat3(("PLight specular " + std::to_string(x)).c_str(), &AssetManagerPtr::GetAssetPtr()->lightManager->PointLightList[x]->LightBuffer.UniformDataInfo.specular.x, 0.0f, 1.0f);
        ImGui::SliderFloat(("PLight constant " + std::to_string(x)).c_str(), &AssetManagerPtr::GetAssetPtr()->lightManager->PointLightList[x]->LightBuffer.UniformDataInfo.constant, 0.0f, 1.0f);
        ImGui::SliderFloat(("PLight linear " + std::to_string(x)).c_str(), &AssetManagerPtr::GetAssetPtr()->lightManager->PointLightList[x]->LightBuffer.UniformDataInfo.linear, 0.0f, 1.0f);
        ImGui::SliderFloat(("PLight quadratic " + std::to_string(x)).c_str(), &AssetManagerPtr::GetAssetPtr()->lightManager->PointLightList[x]->LightBuffer.UniformDataInfo.quadratic, 0.0f, 1.0f);
        ImGui::LabelText("______", "______");
    }

    ImGui::LabelText("SpotLight Light", "Directional Light");
    for (int x = 0; x < AssetManagerPtr::GetAssetPtr()->lightManager->SpotLightList.size(); x++)
    {
        ImGui::SliderFloat3(("SLight position " + std::to_string(x)).c_str(), &AssetManagerPtr::GetAssetPtr()->lightManager->SpotLightList[x]->LightBuffer.UniformDataInfo.position.x, -10.0f, 10.0f);
        ImGui::SliderFloat3(("SLight direction " + std::to_string(x)).c_str(), &AssetManagerPtr::GetAssetPtr()->lightManager->SpotLightList[x]->LightBuffer.UniformDataInfo.direction.x, -1.0f, 1.0f);
        ImGui::SliderFloat3(("SLight ambient " + std::to_string(x)).c_str(), &AssetManagerPtr::GetAssetPtr()->lightManager->SpotLightList[x]->LightBuffer.UniformDataInfo.ambient.x, 0.0f, 1.0f);
        ImGui::SliderFloat3(("SLight Diffuse " + std::to_string(x)).c_str(), &AssetManagerPtr::GetAssetPtr()->lightManager->SpotLightList[x]->LightBuffer.UniformDataInfo.diffuse.x, 0.0f, 1.0f);
        ImGui::SliderFloat3(("SLight specular " + std::to_string(x)).c_str(), &AssetManagerPtr::GetAssetPtr()->lightManager->SpotLightList[x]->LightBuffer.UniformDataInfo.specular.x, 0.0f, 1.0f);
        ImGui::SliderFloat(("SLight constant " + std::to_string(x)).c_str(), &AssetManagerPtr::GetAssetPtr()->lightManager->SpotLightList[x]->LightBuffer.UniformDataInfo.constant, 0.0f, 1.0f);
        ImGui::SliderFloat(("SLight linear " + std::to_string(x)).c_str(), &AssetManagerPtr::GetAssetPtr()->lightManager->SpotLightList[x]->LightBuffer.UniformDataInfo.linear, 0.0f, 1.0f);
        ImGui::SliderFloat(("SLight quadratic " + std::to_string(x)).c_str(), &AssetManagerPtr::GetAssetPtr()->lightManager->SpotLightList[x]->LightBuffer.UniformDataInfo.quadratic, 0.0f, 1.0f);
        ImGui::LabelText("______", "______");
    }
}

void RayTraceRenderer::Draw()
{
    rayTraceRenderPass.Draw(EnginePtr::GetEnginePtr(), AssetManagerPtr::GetAssetPtr(), AssetManagerPtr::GetAssetPtr()->cameraManager->ActiveCamera, rendererID);
    FrameBufferRenderer.Draw();
}

void RayTraceRenderer::Destroy()
{
    rayTraceRenderPass.Destroy(EnginePtr::GetEnginePtr());
    FrameBufferRenderer.Destroy();
}

std::vector<VkCommandBuffer> RayTraceRenderer::AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList)
{
    CommandBufferSubmitList.emplace_back(rayTraceRenderPass.RayTraceCommandBuffer);
    CommandBufferSubmitList.emplace_back(FrameBufferRenderer.CommandBuffer);
    return CommandBufferSubmitList;
}
