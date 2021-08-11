#include "PBRRenderer.h"

PBRRenderer::PBRRenderer() : BaseRenderer()
{
}

PBRRenderer::PBRRenderer(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window, std::shared_ptr<AssetManager> assetManagerPtr) : BaseRenderer(engine, WindowPtr::GetWindowPtr(), AssetManagerPtr::GetAssetPtr())
{
    cubeMapRenderer = CubeMapRenderPass(512.0f);
    prefilterRenderPass = PrefilterRenderPass(512.0f);
    brdfRenderPass = BRDFRenderPass(cubeMapRenderer.BlurredSkyBoxTexture);
    FrameBufferTextureRenderer = PBRFrameBufferTextureRenderPass(engine, assetManager, cubeMapRenderer.BlurredSkyBoxTexture, prefilterRenderPass.BlurredSkyBoxTexture, brdfRenderPass.BRDFTexture);
    DebugDepthRenderer = DepthDebugRenderPass(FrameBufferTextureRenderer.DepthTexture);
    FrameBufferRenderer = FrameBufferRenderPass(FrameBufferTextureRenderer.RenderedTexture, FrameBufferTextureRenderer.BloomTexture);
}

PBRRenderer::~PBRRenderer()
{
}

void PBRRenderer::RebuildSwapChain(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window)
{
    cubeMapRenderer.RebuildSwapChain();
    prefilterRenderPass.RebuildSwapChain();
    brdfRenderPass.RebuildSwapChain(cubeMapRenderer.BlurredSkyBoxTexture);
    FrameBufferTextureRenderer.RebuildSwapChain(engine, assetManager, cubeMapRenderer.BlurredSkyBoxTexture, prefilterRenderPass.BlurredSkyBoxTexture, brdfRenderPass.BRDFTexture);
    DebugDepthRenderer.RebuildSwapChain(FrameBufferTextureRenderer.DepthTexture);
    FrameBufferRenderer.RebuildSwapChain(FrameBufferTextureRenderer.RenderedTexture, FrameBufferTextureRenderer.BloomTexture);
}

void PBRRenderer::GUIUpdate(std::shared_ptr<VulkanEngine> engine)
{
    ImGui::LabelText("Material", "Material");
    for (int x = 0; x < assetManager->materialManager->MaterialList.size(); x++)
    {
        ImGui::SliderFloat3(("Material Albedo " + std::to_string(x)).c_str(), &assetManager->materialManager->MaterialList[x]->materialTexture.Albedo.x, 0.0f, 1.0f);
        ImGui::SliderFloat(("Material Metallic " + std::to_string(x)).c_str(), &assetManager->materialManager->MaterialList[x]->materialTexture.Matallic, 0.0f, 1.0f);
        ImGui::SliderFloat(("Material Roughness " + std::to_string(x)).c_str(), &assetManager->materialManager->MaterialList[x]->materialTexture.Roughness, 0.0f, 1.0f);
        ImGui::SliderFloat(("Material AmbientOcclusion " + std::to_string(x)).c_str(), &assetManager->materialManager->MaterialList[x]->materialTexture.AmbientOcclusion, 0.0f, 1.0f);
        ImGui::SliderFloat(("Material Alpha " + std::to_string(x)).c_str(), &assetManager->materialManager->MaterialList[x]->materialTexture.Alpha, 0.0f, 1.0f);
        ImGui::LabelText("______", "______");
    }

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
    ImGui::Image(FrameBufferTextureRenderer.RenderedTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    ImGui::Image(FrameBufferTextureRenderer.BloomTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    ImGui::Image(DebugDepthRenderer.DebugDepthTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    ImGui::Image(cubeMapRenderer.RenderedTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    ImGui::Image(prefilterRenderPass.RenderedTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    ImGui::Image(brdfRenderPass.BRDFTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
}

void PBRRenderer::Draw(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window, uint32_t imageIndex)
{
    FrameBufferTextureRenderer.Draw(engine, assetManager, rendererID);
    DebugDepthRenderer.Draw();
    FrameBufferRenderer.Draw();
}

void PBRRenderer::Destroy(std::shared_ptr<VulkanEngine> engine)
{
    cubeMapRenderer.Destroy();
    prefilterRenderPass.Destroy();
    brdfRenderPass.Destroy();
    FrameBufferTextureRenderer.Destroy(engine);
    DebugDepthRenderer.Destroy();
    FrameBufferRenderer.Destroy();
}

std::vector<VkCommandBuffer> PBRRenderer::AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList)
{
    CommandBufferSubmitList.emplace_back(FrameBufferTextureRenderer.CommandBuffer);
    CommandBufferSubmitList.emplace_back(DebugDepthRenderer.CommandBuffer);
    CommandBufferSubmitList.emplace_back(FrameBufferRenderer.CommandBuffer);
    return CommandBufferSubmitList;
}
