#include "HybridRenderer.h"

HybridRenderer::HybridRenderer() : BaseRenderer()
{
}

HybridRenderer::HybridRenderer(std::shared_ptr<VulkanEngine> engine) : BaseRenderer()
{
    FrameBufferTextureRenderer = GBufferRenderPass(engine);
    FrameBufferTextureRenderer2 = GBufferRenderPass2(engine);
    rayTraceRenderPass = RayTraceRenderPass(engine, AssetManagerPtr::GetAssetPtr());
    bloomRenderPass = BloomRenderPass(FrameBufferTextureRenderer.GBloomTexture);
   // DebugDepthRenderer = DepthDebugRenderPass(engine, assetManager, FrameBufferTextureRenderer.DepthTexture);

    SSAOTextureList textures = {};
    textures.GPositionTexture = FrameBufferTextureRenderer.GPositionTexture;
    textures.GNormalTexture = FrameBufferTextureRenderer.GNormalTexture;
    textures.NormalMapTexture = FrameBufferTextureRenderer.NormalMapTexture;
    SSAORenderer = SSAORenderPass(textures);
    SSAOBlurRenderer = SSAOBlurRenderPass(SSAORenderer.SSAOTexture);

    HybridFrameBufferTextures frameBufferTextures{};
    frameBufferTextures.AlebdoTexture = FrameBufferTextureRenderer.GAlbedoTexture;
    frameBufferTextures.PositionTexture = FrameBufferTextureRenderer.GPositionTexture;
    frameBufferTextures.NormalTexture = FrameBufferTextureRenderer.GNormalTexture;
    frameBufferTextures.BloomTexture = bloomRenderPass.BloomTexture;
    frameBufferTextures.SSA0Texture = SSAOBlurRenderer.SSAOBlurTexture;
    frameBufferTextures.ShadowTexture = rayTraceRenderPass.ShadowTextureMask;
    frameBufferTextures.ReflectionTexture = rayTraceRenderPass.ReflectionTexture;
    frameBufferTextures.SkyBoxTexture = rayTraceRenderPass.ReflectionTexture;
    frameBufferTextures.NormalMapTexture = FrameBufferTextureRenderer.NormalMapTexture;
    frameBufferTextures.SpecularMapTexture = FrameBufferTextureRenderer.SpecularMapTexture;
    frameBufferTextures.ReflectionMapTexture = FrameBufferTextureRenderer2.NormalMapTexture;
    frameBufferTextures.TBNTangentTexture = FrameBufferTextureRenderer2.GTBN_TangentTexture;
    frameBufferTextures.TBNBiTangentTexture = FrameBufferTextureRenderer2.GTBN_BiTangentTexture;
    frameBufferTextures.TBNNormalMapTexture = FrameBufferTextureRenderer2.GTBN_NormalTexture;
    FrameBufferRenderer = HybridFrameBufferRenderPass(frameBufferTextures);

    CurrentSSAOSampleRate = SSAORenderer.KernalSampleSize;
}

HybridRenderer::~HybridRenderer()
{
}

void HybridRenderer::RebuildSwapChain()
{
    SSAOTextureList textures = {};
    textures.GPositionTexture = FrameBufferTextureRenderer.GPositionTexture;
    textures.GNormalTexture = FrameBufferTextureRenderer.GNormalTexture;
    textures.NormalMapTexture = FrameBufferTextureRenderer.NormalMapTexture;

    HybridFrameBufferTextures frameBufferTextures{};
    frameBufferTextures.AlebdoTexture = FrameBufferTextureRenderer.GAlbedoTexture;
    frameBufferTextures.PositionTexture = FrameBufferTextureRenderer.GPositionTexture;
    frameBufferTextures.NormalTexture = FrameBufferTextureRenderer.GNormalTexture;
    frameBufferTextures.BloomTexture = bloomRenderPass.BloomTexture;
    frameBufferTextures.SSA0Texture = SSAOBlurRenderer.SSAOBlurTexture;
    frameBufferTextures.ShadowTexture = rayTraceRenderPass.ShadowTextureMask;
    frameBufferTextures.ReflectionTexture = rayTraceRenderPass.ReflectionTexture;
    frameBufferTextures.SkyBoxTexture = rayTraceRenderPass.ReflectionTexture;
    frameBufferTextures.NormalMapTexture = FrameBufferTextureRenderer.NormalMapTexture;
    frameBufferTextures.SpecularMapTexture = FrameBufferTextureRenderer.SpecularMapTexture;
    frameBufferTextures.ReflectionMapTexture = FrameBufferTextureRenderer2.NormalMapTexture;
    frameBufferTextures.TBNTangentTexture = FrameBufferTextureRenderer2.GTBN_TangentTexture;
    frameBufferTextures.TBNBiTangentTexture = FrameBufferTextureRenderer2.GTBN_BiTangentTexture;
    frameBufferTextures.TBNNormalMapTexture = FrameBufferTextureRenderer2.GTBN_NormalTexture;
    FrameBufferTextureRenderer.RebuildSwapChain();
    FrameBufferTextureRenderer2.RebuildSwapChain();
    rayTraceRenderPass.RebuildSwapChain(EnginePtr::GetEnginePtr(), AssetManagerPtr::GetAssetPtr());
    bloomRenderPass.RebuildSwapChain(FrameBufferTextureRenderer.GBloomTexture);
   // DebugDepthRenderer.RebuildSwapChain(engine, assetManager, FrameBufferTextureRenderer.DepthTexture);
    SSAORenderer.RebuildSwapChain(textures);
    SSAOBlurRenderer.RebuildSwapChain(SSAORenderer.SSAOTexture);
    FrameBufferRenderer.RebuildSwapChain(frameBufferTextures);
}

void HybridRenderer::GUIUpdate()
{
    rayTraceRenderPass.GUIUpdate();

    ImGui::Image(FrameBufferTextureRenderer2.NormalMapTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
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

    ImGui::Checkbox("Apply SSAO", &ApplySSAO);
    ImGui::SliderInt("Apply SSAO", &AssetManagerPtr::GetAssetPtr()->SceneData->UniformDataInfo.temp, 0, 1);
   // ImGui::SliderFloat3("DirectionalLight", &assetManager->SceneData->UniformDataInfo.dlight.direction.x, -1.0f, 1.0f);
    ImGui::SliderInt("SSAOSample", &SSAORenderer.KernalSampleSize, 0, 2550);
    ImGui::SliderFloat("SSAOBias", &SSAORenderer.bias, 0.0f, 100.0f);
    ImGui::SliderFloat("SSAORadius", &SSAORenderer.radius, 0.0f, 10000000.0f);
    ImGui::Image(FrameBufferTextureRenderer.GPositionTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    ImGui::Image(FrameBufferTextureRenderer.GAlbedoTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    ImGui::Image(FrameBufferTextureRenderer.GNormalTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    ImGui::Image(FrameBufferTextureRenderer.GBloomTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    ImGui::Image(FrameBufferTextureRenderer.NormalMapTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    ImGui::Image(FrameBufferTextureRenderer.SpecularMapTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    ImGui::Image(bloomRenderPass.BloomTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    //ImGui::Image(DebugDepthRenderer.DebugDepthTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    if (ApplySSAO)
    {
        ImGui::Image(SSAORenderer.SSAOTexture->ImGuiDescriptorSet, ImVec2(180.0f * 4, 180.0f * 4));
        ImGui::Image(SSAOBlurRenderer.SSAOBlurTexture->ImGuiDescriptorSet, ImVec2(180.0f * 4, 180.0f * 4));
    }
    ImGui::Image(FrameBufferTextureRenderer.DepthTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));

    if (CurrentSSAOSampleRate != SSAORenderer.KernalSampleSize)
    {
        SSAOTextureList textures = {};
        textures.GPositionTexture = FrameBufferTextureRenderer.GPositionTexture;
        textures.GNormalTexture = FrameBufferTextureRenderer.GNormalTexture;
        SSAORenderer.RebuildSwapChain(textures);

        CurrentSSAOSampleRate = SSAORenderer.KernalSampleSize;
    }
}

void HybridRenderer::Draw()
{
    FrameBufferTextureRenderer.Draw();
    FrameBufferTextureRenderer2.Draw();
    rayTraceRenderPass.Draw(EnginePtr::GetEnginePtr(), AssetManagerPtr::GetAssetPtr(), rendererID, AssetManagerPtr::GetAssetPtr()->cameraManager->ActiveCamera);
    bloomRenderPass.Draw();
   // DebugDepthRenderer.Draw(engine, assetManager, imageIndex);
    if (ApplySSAO)
    {
        SSAORenderer.Draw();
        SSAOBlurRenderer.Draw();
    }
    FrameBufferRenderer.Draw(rendererID);
}

void HybridRenderer::Destroy()
{
    FrameBufferTextureRenderer.Destroy();
    FrameBufferTextureRenderer2.Destroy();
    rayTraceRenderPass.Destroy(EnginePtr::GetEnginePtr());
    bloomRenderPass.Destroy();
    //DebugDepthRenderer.Destroy(engine);
    SSAORenderer.Destroy();
    SSAOBlurRenderer.Destroy();
    FrameBufferRenderer.Destroy();
}

std::vector<VkCommandBuffer> HybridRenderer::AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList)
{
    CommandBufferSubmitList.emplace_back(FrameBufferTextureRenderer.CommandBuffer);
    CommandBufferSubmitList.emplace_back(FrameBufferTextureRenderer2.CommandBuffer);
    CommandBufferSubmitList.emplace_back(rayTraceRenderPass.RayTraceCommandBuffer);
    CommandBufferSubmitList.emplace_back(bloomRenderPass.CommandBuffer);
   // CommandBufferSubmitList.emplace_back(DebugDepthRenderer.CommandBuffer);
    if (ApplySSAO)
    {
        CommandBufferSubmitList.emplace_back(SSAORenderer.CommandBuffer);
        CommandBufferSubmitList.emplace_back(SSAOBlurRenderer.CommandBuffer);
    }
    CommandBufferSubmitList.emplace_back(FrameBufferRenderer.CommandBuffer);
    return CommandBufferSubmitList;
}
