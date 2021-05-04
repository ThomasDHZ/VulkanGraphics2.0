#include "PBRRenderer.h"

PBRRenderer::PBRRenderer() : BaseRenderer()
{
}

PBRRenderer::PBRRenderer(VulkanEngine& engine, VulkanWindow& window, std::shared_ptr<AssetManager> assetManagerPtr) : BaseRenderer(engine, window, assetManagerPtr)
{
    forwardRenderPass = ForwardRenderPass(engine, assetManager, assetManager->SceneData, assetManager->SkyUniformBuffer);
    cubeMapRenderer = CubeMapRenderPass(engine, assetManager, 512.0f, assetManager->SceneData, assetManager->SkyUniformBuffer);
    prefilterRenderPass = PrefilterRenderPass(engine, assetManager, 512.0f, assetManager->SceneData, assetManager->SkyUniformBuffer);

    ImGui_ImplVulkan_AddTexture(cubeMapRenderer.RenderedTexture->ImGuiDescriptorSet, cubeMapRenderer.RenderedTexture->Sampler, cubeMapRenderer.RenderedTexture->View, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    ImGui_ImplVulkan_AddTexture(prefilterRenderPass.RenderedTexture->ImGuiDescriptorSet, prefilterRenderPass.RenderedTexture->Sampler, prefilterRenderPass.RenderedTexture->View, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

}

PBRRenderer::~PBRRenderer()
{
}

void PBRRenderer::RebuildSwapChain(VulkanEngine& engine, VulkanWindow& window)
{
    forwardRenderPass.RebuildSwapChain(engine, assetManager, assetManager->SceneData, assetManager->SkyUniformBuffer);
    cubeMapRenderer.RebuildSwapChain(engine);
    prefilterRenderPass.RebuildSwapChain(engine);
}

void PBRRenderer::Update(VulkanEngine& engine, VulkanWindow& window, uint32_t currentImage)
{
}

void PBRRenderer::GUIUpdate(VulkanEngine& engine)
{
    ImGui::Image(cubeMapRenderer.RenderedTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    ImGui::Image(prefilterRenderPass.RenderedTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    ImGui::SliderInt("PrefilterSameCount", &assetManager->SceneData->UniformDataInfo.temp, 0, 1);

    ImGui::LabelText("Orginal View", "Orginal View");
    //ImGui::Image(cubeMapRenderer.RenderedTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    //ImGui::Image(prefilterRenderPass.RenderedTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    for (int x = 0; x < 6; x++)
    {
        /* ImGui::LabelText(("Copy texture " + std::to_string(x + 1)).c_str(), ("Copy texture " + std::to_string(x + 1)).c_str());
         ImGui::Image(cubeMapRenderer.CopyTextureList[x]->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));*/
    }

    ImGui::SliderFloat3("Pos", &assetManager->SceneData->UniformDataInfo.dlight.direction.x, -1.0f, 1.0f);
    ImGui::SliderFloat3("Diffuse", &assetManager->SceneData->UniformDataInfo.dlight.diffuse.x, 0.0f, 1.0f);

    ImGui::SliderFloat3("Pos20", &assetManager->SceneData->UniformDataInfo.plight[0].position.x, -10.0f, 10.0f);
    ImGui::SliderFloat3("Diffuse20", &assetManager->SceneData->UniformDataInfo.plight[0].diffuse.x, 0.0f, 1.0f);

    ImGui::SliderFloat3("Pos21", &assetManager->SceneData->UniformDataInfo.plight[1].position.x, -10.0f, 10.0f);
    ImGui::SliderFloat3("Diffuse21", &assetManager->SceneData->UniformDataInfo.plight[1].diffuse.x, 0.0f, 1.0f);

    ImGui::SliderFloat3("Pos22", &assetManager->SceneData->UniformDataInfo.plight[2].position.x, -10.0f, 10.0f);
    ImGui::SliderFloat3("Diffuse22", &assetManager->SceneData->UniformDataInfo.plight[2].diffuse.x, 0.0f, 1.0f);

    ImGui::SliderFloat3("Pos23", &assetManager->SceneData->UniformDataInfo.plight[3].position.x, -10.0f, 10.0f);
    ImGui::SliderFloat3("Diffuse23", &assetManager->SceneData->UniformDataInfo.plight[3].diffuse.x, 0.0f, 1.0f);

    ImGui::SliderFloat3("Pos24", &assetManager->SceneData->UniformDataInfo.plight[4].position.x, -10.0f, 10.0f);
    ImGui::SliderFloat3("Speculare24", &assetManager->SceneData->UniformDataInfo.plight[4].specular.x, 0.0f, 1.0f);


    for (int y = 0; y < assetManager->meshManager.MeshList.size(); y++)
    {
        auto a = std::to_string(y);
        ImGui::Checkbox(a.c_str(), &assetManager->meshManager.MeshList[y]->ShowMesh);

        ImGui::SliderFloat(("Depth " + std::to_string(y)).c_str(), &assetManager->meshManager.MeshList[y]->MeshProperties.UniformDataInfo.heightScale, 0.0f, 1.0f);
        ImGui::SliderFloat2(("UV Offset " + std::to_string(y)).c_str(), &assetManager->meshManager.MeshList[y]->MeshProperties.UniformDataInfo.UVOffset.x, 0.0f, 1.0f);
        ImGui::SliderFloat3(("Transform " + std::to_string(y)).c_str(), &assetManager->meshManager.MeshList[y]->MeshPosition.x, -1000.0f, 1000.0f);
        ImGui::SliderFloat3(("Rotate " + std::to_string(y)).c_str(), &assetManager->meshManager.MeshList[y]->MeshRotation.x, 0.0f, 360.0f);
        ImGui::SliderFloat3(("Scale " + std::to_string(y)).c_str(), &assetManager->meshManager.MeshList[y]->MeshScale.x, 0.0f, 3.0f);
    }

    // ImGui::SliderFloat3("ReflectCamPos", &waterReflectionRenderPass.TextureCamera->Position.x, -10.0f, 10.0f);
    // ImGui::SliderFloat("ReflectPitch", &waterReflectionRenderPass.TextureCamera->Pitch, -180.0f, 180.0f);
    // ImGui::SliderFloat("ReflectYaw", &waterReflectionRenderPass.TextureCamera->Yaw, -180.0f, 180.0f);
    //// ImGui::Image(waterReflectionRenderPass.RenderedTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));

    // ImGui::SliderFloat3("RefractionCamPos", &waterRefractionRenderPass.TextureCamera->Position.x, -10.0f, 10.0f);
    // ImGui::SliderFloat("RefractionPitch", &waterRefractionRenderPass.TextureCamera->Pitch, -180.0f, 180.0f);
    // ImGui::SliderFloat("RefractionYaw", &waterRefractionRenderPass.TextureCamera->Yaw, -180.0f, 180.0f);
    // ImGui::Image(waterRefractionRenderPass.RenderedTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
}

void PBRRenderer::Draw(VulkanEngine& engine, VulkanWindow& window, uint32_t imageIndex)
{
    forwardRenderPass.Draw(engine, assetManager, imageIndex, rendererID);
    cubeMapRenderer.Draw(engine, assetManager, imageIndex);
    prefilterRenderPass.Draw(engine, assetManager, imageIndex);
}

void PBRRenderer::Destroy(VulkanEngine& engine)
{
    cubeMapRenderer.Destroy(engine);
    prefilterRenderPass.Destroy(engine);
    forwardRenderPass.Destroy(engine);
}

std::vector<VkCommandBuffer> PBRRenderer::AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList)
{
    CommandBufferSubmitList.emplace_back(forwardRenderPass.CommandBuffer);
    CommandBufferSubmitList.emplace_back(cubeMapRenderer.CommandBuffer);
    CommandBufferSubmitList.emplace_back(prefilterRenderPass.CommandBuffer);
    return CommandBufferSubmitList;
}
