#include "BlinnPhongRasterRenderer.h"
#include "BillboardMesh.h"

BlinnPhongRasterRenderer::BlinnPhongRasterRenderer() : BaseRenderer()
{
}

BlinnPhongRasterRenderer::BlinnPhongRasterRenderer(VulkanEngine& engine, std::shared_ptr<VulkanWindow> window, std::shared_ptr<AssetManager> assetManagerPtr) : BaseRenderer(engine, window, assetManagerPtr)
{
    text = TextRenderPass(engine, assetManager);
    FrameBufferTextureRenderer = FrameBufferTextureRenderPass(engine, assetManager);
    //BloomRenderer = BloomRenderPass(engine, assetManager, FrameBufferTextureRenderer.BloomTexture);
    //DebugDepthRenderer = DepthDebugRenderPass(engine, assetManager, FrameBufferTextureRenderer.DepthTexture);
    FrameBufferRenderer = FrameBufferRenderPass(engine, assetManager, text.RenderedTexture, text.RenderedTexture);
    //lightPathRenderer = DepthRenderer(engine, assetManager);
}

BlinnPhongRasterRenderer::~BlinnPhongRasterRenderer()
{
}

void BlinnPhongRasterRenderer::RebuildSwapChain(VulkanEngine& engine, std::shared_ptr<VulkanWindow> window)
{
    FrameBufferTextureRenderer.RebuildSwapChain(engine, assetManager);
    //BloomRenderer.RebuildSwapChain(engine, assetManager, FrameBufferTextureRenderer.BloomTexture);
    //DebugDepthRenderer.RebuildSwapChain(engine, assetManager, FrameBufferTextureRenderer.DepthTexture);
    //FrameBufferRenderer.RebuildSwapChain(engine, assetManager, FrameBufferTextureRenderer.RenderedTexture, BloomRenderer.BloomTexture);
    //lightPathRenderer.RebuildSwapChain(engine, assetManager);
}

void BlinnPhongRasterRenderer::GUIUpdate(VulkanEngine& engine)
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

    ImGui::LabelText("Material", "Material");
    for (int x = 0; x < assetManager->materialManager.MaterialList.size(); x++)
    {
        ImGui::SliderFloat3(("Material Diffuse " + std::to_string(x)).c_str(), &assetManager->materialManager.MaterialList[x]->materialTexture.Diffuse.x, 0.0f, 1.0f);
        ImGui::SliderFloat3(("Material Specular " + std::to_string(x)).c_str(), &assetManager->materialManager.MaterialList[x]->materialTexture.Specular.x, 0.0f, 1.0f);
        //ImGui::SliderFloat3(("Material Emission " + std::to_string(x)).c_str(), &assetManager->materialManager.MaterialList[x]->materialTexture..x, 0.0f, 1.0f);
        ImGui::SliderFloat(("Material Shininess " + std::to_string(x)).c_str(), &assetManager->materialManager.MaterialList[x]->materialTexture.Shininess, 0.0f, 1.0f);
        ImGui::SliderFloat(("Material Reflectivness " + std::to_string(x)).c_str(), &assetManager->materialManager.MaterialList[x]->materialTexture.Reflectivness, 0.0f, 1.0f);
        ImGui::SliderFloat(("Material Alpha " + std::to_string(x)).c_str(), &assetManager->materialManager.MaterialList[x]->materialTexture.Alpha, 0.0f, 1.0f);
        ImGui::LabelText("______", "______");
    }

    ImGui::LabelText("Directional Light", "Directional Light");
    for(int x = 0; x < assetManager->lightManager.DirectionalLightList.size(); x++)
    {
        ImGui::SliderFloat3(("DLight direction " + std::to_string(x)).c_str(), &assetManager->lightManager.DirectionalLightList[x]->LightBuffer.UniformDataInfo.direction.x, -1.0f, 1.0f);
        ImGui::SliderFloat3(("DLight ambient " + std::to_string(x)).c_str(), &assetManager->lightManager.DirectionalLightList[x]->LightBuffer.UniformDataInfo.ambient.x, 0.0f, 1.0f);
        ImGui::SliderFloat3(("DLight Diffuse " + std::to_string(x)).c_str(), &assetManager->lightManager.DirectionalLightList[x]->LightBuffer.UniformDataInfo.diffuse.x, 0.0f, 1.0f);
        ImGui::SliderFloat3(("DLight specular " + std::to_string(x)).c_str(), &assetManager->lightManager.DirectionalLightList[x]->LightBuffer.UniformDataInfo.specular.x, 0.0f, 1.0f);
        ImGui::LabelText("______", "______");
    }

    ImGui::LabelText("Point Light", "Point Light");
    for (int x = 0; x < assetManager->lightManager.PointLightList.size(); x++)
    {
        ImGui::SliderFloat3(("PLight position " + std::to_string(x)).c_str(), &assetManager->lightManager.PointLightList[x]->LightBuffer.UniformDataInfo.position.x, -100.0f, 100.0f);
        ImGui::SliderFloat3(("PLight ambient " + std::to_string(x)).c_str(), &assetManager->lightManager.PointLightList[x]->LightBuffer.UniformDataInfo.ambient.x, 0.0f, 1.0f);
        ImGui::SliderFloat3(("PLight Diffuse " + std::to_string(x)).c_str(), &assetManager->lightManager.PointLightList[x]->LightBuffer.UniformDataInfo.diffuse.x, 0.0f, 1.0f);
        ImGui::SliderFloat3(("PLight specular " + std::to_string(x)).c_str(), &assetManager->lightManager.PointLightList[x]->LightBuffer.UniformDataInfo.specular.x, 0.0f, 1.0f);
        ImGui::SliderFloat(("PLight constant " + std::to_string(x)).c_str(), &assetManager->lightManager.PointLightList[x]->LightBuffer.UniformDataInfo.constant, 0.0f, 1.0f);
        ImGui::SliderFloat(("PLight linear " + std::to_string(x)).c_str(), &assetManager->lightManager.PointLightList[x]->LightBuffer.UniformDataInfo.linear, 0.0f, 1.0f);
        ImGui::SliderFloat(("PLight quadratic " + std::to_string(x)).c_str(), &assetManager->lightManager.PointLightList[x]->LightBuffer.UniformDataInfo.quadratic, 0.0f, 1.0f);
        ImGui::LabelText("______", "______");
    }

    ImGui::LabelText("SpotLight Light", "Directional Light");
    for (int x = 0; x < assetManager->lightManager.SpotLightList.size(); x++)
    {
        ImGui::SliderFloat3(("SLight position " + std::to_string(x)).c_str(), &assetManager->lightManager.SpotLightList[x]->LightBuffer.UniformDataInfo.position.x, -10.0f, 10.0f);
        ImGui::SliderFloat3(("SLight direction " + std::to_string(x)).c_str(), &assetManager->lightManager.SpotLightList[x]->LightBuffer.UniformDataInfo.direction.x, -1.0f, 1.0f);
        ImGui::SliderFloat3(("SLight ambient " + std::to_string(x)).c_str(), &assetManager->lightManager.SpotLightList[x]->LightBuffer.UniformDataInfo.ambient.x, 0.0f, 1.0f);
        ImGui::SliderFloat3(("SLight Diffuse " + std::to_string(x)).c_str(), &assetManager->lightManager.SpotLightList[x]->LightBuffer.UniformDataInfo.diffuse.x, 0.0f, 1.0f);
        ImGui::SliderFloat3(("SLight specular " + std::to_string(x)).c_str(), &assetManager->lightManager.SpotLightList[x]->LightBuffer.UniformDataInfo.specular.x, 0.0f, 1.0f);
        ImGui::SliderFloat(("SLight constant " + std::to_string(x)).c_str(), &assetManager->lightManager.SpotLightList[x]->LightBuffer.UniformDataInfo.constant, 0.0f, 1.0f);
        ImGui::SliderFloat(("SLight linear " + std::to_string(x)).c_str(), &assetManager->lightManager.SpotLightList[x]->LightBuffer.UniformDataInfo.linear, 0.0f, 1.0f);
        ImGui::SliderFloat(("SLight quadratic " + std::to_string(x)).c_str(), &assetManager->lightManager.SpotLightList[x]->LightBuffer.UniformDataInfo.quadratic, 0.0f, 1.0f);
        ImGui::LabelText("______", "______");
    }

    //ImGui::Image(FrameBufferTextureRenderer.RenderedTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    //ImGui::Image(FrameBufferTextureRenderer.BloomTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    //ImGui::Image(lightPathRenderer.DepthTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    //ImGui::Image(BloomRenderer.BloomTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
}

void BlinnPhongRasterRenderer::Draw(VulkanEngine& engine, std::shared_ptr<VulkanWindow> window, uint32_t imageIndex)
{
    text.Draw(engine, assetManager, imageIndex, rendererID);
    //FrameBufferTextureRenderer.Draw(engine, assetManager, imageIndex, rendererID);
    //BloomRenderer.Draw(engine, assetManager, imageIndex);
    //DebugDepthRenderer.Draw(engine, assetManager, imageIndex);
    FrameBufferRenderer.Draw(engine, assetManager, imageIndex);
   /* lightPathRenderer.Draw(engine, assetManager, imageIndex);*/
}

void BlinnPhongRasterRenderer::Destroy(VulkanEngine& engine)
{
    FrameBufferTextureRenderer.Destroy(engine);
    BloomRenderer.Destroy(engine);
    DebugDepthRenderer.Destroy(engine);
    FrameBufferRenderer.Destroy(engine);
    lightPathRenderer.Destroy(engine);
}

std::vector<VkCommandBuffer> BlinnPhongRasterRenderer::AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList)
{
    CommandBufferSubmitList.emplace_back(text.CommandBuffer);
    //CommandBufferSubmitList.emplace_back(FrameBufferTextureRenderer.CommandBuffer);
    //CommandBufferSubmitList.emplace_back(BloomRenderer.CommandBuffer);
    //CommandBufferSubmitList.emplace_back(DebugDepthRenderer.CommandBuffer);
    CommandBufferSubmitList.emplace_back(FrameBufferRenderer.CommandBuffer);
    //CommandBufferSubmitList.emplace_back(lightPathRenderer.CommandBuffer);
    return CommandBufferSubmitList;
}