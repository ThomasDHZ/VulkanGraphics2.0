#include "Renderer.h"
#include <stb_image.h>
#include "Sprite.h"
#include "MegaMan.h"
#include "Mario.h"
#include "TerrainMesh.h"


Renderer::Renderer()
{
}

Renderer::Renderer(VulkanEngine& engine, VulkanWindow& window, std::shared_ptr<AssetManager> assetManagerPTR)
{
   // interfaceRenderPass = InterfaceRenderPass(engine, window.GetWindowPtr());
    assetManager = assetManagerPTR;

    pbrRenderer = PBRRenderer(engine, window, assetManager);
  //  frameBufferRenderPass = FrameBufferRenderPass(engine, assetManager, SceneData);
    //gBufferRenderPass = DeferredRenderPass(engine, assetManager, SceneData);
    //textureRenderPass = TextureRenderPass(engine, assetManager, SceneData);

    skybox = Skybox(engine, assetManager, pbrRenderer.forwardRenderPass.RenderPass);
    RayRenderer = RayTraceRenderPass(engine, assetManager, assetManager->SceneData);


    assetManager->SceneData->UniformDataInfo.dlight.direction = glm::vec4(0.0f);
    assetManager->SceneData->UniformDataInfo.dlight.ambient = glm::vec4(0.2f);
    assetManager->SceneData->UniformDataInfo.dlight.diffuse = glm::vec4(0.5f);
    assetManager->SceneData->UniformDataInfo.dlight.specular = glm::vec4(1.0f);

    assetManager->SceneData->UniformDataInfo.plight[0].position = glm::vec4(0.5f, 1.0f, 0.3f, 1.0f);
    assetManager->SceneData->UniformDataInfo.plight[0].ambient = glm::vec4(0.2f);
    assetManager->SceneData->UniformDataInfo.plight[0].diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 0.0f);
    assetManager->SceneData->UniformDataInfo.plight[0].specular = glm::vec4(1.0f);

    assetManager->SceneData->UniformDataInfo.plight[1].position = glm::vec4(0.5f, 1.0f, 0.3f, 1.0f);
    assetManager->SceneData->UniformDataInfo.plight[1].ambient = glm::vec4(0.2f);
    assetManager->SceneData->UniformDataInfo.plight[1].diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 0.0f);
    assetManager->SceneData->UniformDataInfo.plight[1].specular = glm::vec4(1.0f);

    assetManager->SceneData->UniformDataInfo.plight[2].position = glm::vec4(0.5f, 1.0f, 0.3f, 1.0f);
    assetManager->SceneData->UniformDataInfo.plight[2].ambient = glm::vec4(0.2f);
    assetManager->SceneData->UniformDataInfo.plight[2].diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 0.0f);
    assetManager->SceneData->UniformDataInfo.plight[2].specular = glm::vec4(1.0f);

    assetManager->SceneData->UniformDataInfo.plight[3].position = glm::vec4(0.5f, 1.0f, 0.3f, 1.0f);
    assetManager->SceneData->UniformDataInfo.plight[3].ambient = glm::vec4(0.2f);
    assetManager->SceneData->UniformDataInfo.plight[3].diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 0.0f);
    assetManager->SceneData->UniformDataInfo.plight[3].specular = glm::vec4(1.0f);

    assetManager->SceneData->UniformDataInfo.plight[4].position = glm::vec4(0.5f, 1.0f, 0.3f, 1.0f);
    assetManager->SceneData->UniformDataInfo.plight[4].ambient = glm::vec4(0.2f);
    assetManager->SceneData->UniformDataInfo.plight[4].diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 0.0f);
    assetManager->SceneData->UniformDataInfo.plight[4].specular = glm::vec4(1.0f);

    assetManager->SceneData->UniformDataInfo.sLight.ambient = glm::vec4(0.0f);
    assetManager->SceneData->UniformDataInfo.sLight.diffuse = glm::vec4(1.0f);
    assetManager->SceneData->UniformDataInfo.sLight.specular = glm::vec4(1.0f);

    //assetManager.textureManager.LoadTexture2D(waterReflectionRenderPass.RenderedTexture);
    //assetManager.textureManager.LoadTexture2D(waterRefractionRenderPass.RenderedTexture);


    //ImGui_ImplVulkan_AddTexture(waterRenderPass.ReflectionTexture->ImGuiDescriptorSet, waterRenderPass.ReflectionTexture->Sampler, waterRenderPass.ReflectionTexture->View, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    //mGui_ImplVulkan_AddTexture(waterRenderPass.RefractionTexture->ImGuiDescriptorSet, waterRenderPass.RefractionTexture->Sampler, waterRenderPass.RefractionTexture->View, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
   //ImGui_ImplVulkan_AddTexture(cubeMapRenderer.RenderedTexture->ImGuiDescriptorSet, cubeMapRenderer.RenderedTexture->Sampler, cubeMapRenderer.RenderedTexture->View, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
  // ImGui_ImplVulkan_AddTexture(prefilterRenderPass.RenderedTexture->ImGuiDescriptorSet, prefilterRenderPass.RenderedTexture->Sampler, prefilterRenderPass.RenderedTexture->View, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

   //for (auto& texture : cubeMapRenderer.CopyTextureList)
   //{
   //    ImGui_ImplVulkan_AddTexture(texture->ImGuiDescriptorSet, texture->Sampler, texture->View, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
   //}
}

Renderer::~Renderer()
{

}

void Renderer::RebuildSwapChain(VulkanEngine& engine, VulkanWindow& window)
{
    //assetManager->textureManager.LoadCubeMap(engine, prefilterRenderPass.BlurredSkyBoxTexture);

    int width = 0, height = 0;
    glfwGetFramebufferSize(window.GetWindowPtr(), &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(window.GetWindowPtr(), &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(engine.Device);

    for (auto imageView : engine.SwapChain.GetSwapChainImageViews()) {
        vkDestroyImageView(engine.Device, imageView, nullptr);
    }

    vkDestroySwapchainKHR(engine.Device, engine.SwapChain.GetSwapChain(), nullptr);

    pbrRenderer.RebuildSwapChain(engine, window);
    //engine.SwapChain.RebuildSwapChain(window.GetWindowPtr(), engine.Device, engine.PhysicalDevice, engine.Surface);
   // gBufferRenderPass.UpdateSwapChain(engine, assetManager, SceneData);
  //  forwardRenderPass.RebuildSwapChain(engine, assetManager, assetManager->SceneData, assetManager->SkyUniformBuffer);
    //cubeMapRenderer.RebuildSwapChain(engine);
    //prefilterRenderPass.RebuildSwapChain(engine);
    //frameBufferRenderPass.UpdateSwapChain(engine, assetManager, SceneData);
    //interfaceRenderPass.RebuildSwapChain(engine);
    //for (auto& mesh : assetManager->meshManager.MeshList)
    //{
    //    if (mesh->MeshType == MeshTypeFlag::Mesh_Type_Water)
    //    {
    //        static_cast<WaterSurfaceMesh*>(mesh.get())->RebuildSwapChain(engine, assetManager, pbrRenderer.forwardRenderPass.RenderPass, assetManager->SceneData);
    //    }
    //}
   // static_cast<WaterSurfaceMesh*>(assetManager.meshManager.MeshList[14].get())->UpdateGraphicsPipeLine(engine, assetManager, forwardRenderPass.RenderPass, SceneData, waterReflectionRenderPass.RenderedTexture, waterRefractionRenderPass.RenderedTexture);
   // textureRenderPass.UpdateSwapChain(engine, assetManager, SceneData);
   RayRenderer.RebuildSwapChain(engine, assetManager, assetManager->SceneData, 0);
}

void Renderer::Update(VulkanEngine& engine, VulkanWindow& window, uint32_t currentImage)
{
    if(UpdateRenderer)
    {
        RebuildSwapChain(engine, window);
    }

    assetManager->Update(engine);
    skybox.Update(engine, assetManager->materialManager);
    RayRenderer.createTopLevelAccelerationStructure(engine, assetManager);

   // WaterRenderPass.Update(engine, assetManager, *SceneData.get(), camera2);
    //for (auto& mesh : assetManager->meshManager.MeshList)
    //{
    //    if (mesh->MeshType == MeshTypeFlag::Mesh_Type_Water)
    //    {
    //        static_cast<WaterSurfaceMesh*>(mesh.get())->Update(engine, assetManager, *assetManager->SceneData.get(), assetManager->camera);
    //    }
    //}
}

void Renderer::GUIUpdate(VulkanEngine& engine)
{
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Checkbox("RayTraceSwitch", &RayTraceSwitch);
    pbrRenderer.GUIUpdate(engine);
}

void Renderer::Draw(VulkanEngine& engine, VulkanWindow& window)
{
    vkWaitForFences(engine.Device, 1, &engine.inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(engine.Device, engine.SwapChain.GetSwapChain(), UINT64_MAX, engine.vulkanSemaphores[currentFrame].ImageAcquiredSemaphore, VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        RebuildSwapChain(engine, window);
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    Update(engine, window, imageIndex);

    if (engine.imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(engine.Device, 1, &engine.imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
    }

    engine.imagesInFlight[imageIndex] = engine.inFlightFences[currentFrame];


    /// <summary>
    /// Draw Area
    /// </summary>
  // gBufferRenderPass.Draw(engine, assetManager, imageIndex);
    for (auto& mesh : assetManager->meshManager.MeshList)
    {
        if (mesh->MeshType == MeshTypeFlag::Mesh_Type_Water)
        {
            static_cast<WaterSurfaceMesh*>(mesh.get())->DrawWaterTexture(engine, assetManager, imageIndex, skybox);
        }
    }
    pbrRenderer.Draw(engine, window, imageIndex, skybox);
    //forwardRenderPass.Draw(engine, assetManager, imageIndex, skybox);
    //cubeMapRenderer.Draw(engine, assetManager, imageIndex, skybox);
    //prefilterRenderPass.Draw(engine, assetManager, imageIndex, skybox);
   //WaterRenderPass.Draw(engine, assetManager, imageIndex);
   //frameBufferRenderPass.Draw(engine, RasterCommandBuffer, imageIndex);
   RayRenderer.Draw(engine, assetManager, imageIndex);
   
    ///
    ///Draw area
    /// 

    VkSemaphore waitSemaphores[] = { engine.vulkanSemaphores[currentFrame].ImageAcquiredSemaphore };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    for (auto& model : assetManager->modelManager.ModelList)
    {
        model->SubmitToCommandBuffer(engine, CommandBufferSubmitList, imageIndex);
    }
    if (RayTraceSwitch)
    {
        pbrRenderer.AddToCommandBufferSubmitList(CommandBufferSubmitList);
        //CommandBufferSubmitList.emplace_back(RayRenderer.RayTraceCommandBuffer);
       // CommandBufferSubmitList.emplace_back(gBufferRenderPass.CommandBuffer);
   
        for (auto& mesh : assetManager->meshManager.MeshList)
        {
            if (mesh->MeshType == MeshTypeFlag::Mesh_Type_Water)
            {
                static_cast<WaterSurfaceMesh*>(mesh.get())->SubmitToCMDBuffer(engine, CommandBufferSubmitList, imageIndex);
            }
        }
    }
    else
    {
        CommandBufferSubmitList.emplace_back(RayRenderer.RayTraceCommandBuffer);
    }
    

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = static_cast<uint32_t>(CommandBufferSubmitList.size());
    submitInfo.pCommandBuffers = CommandBufferSubmitList.data();

    VkSemaphore signalSemaphores[] = { engine.vulkanSemaphores[currentFrame].RenderCompleteSemaphore };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences(engine.Device, 1, &engine.inFlightFences[currentFrame]);

    if (vkQueueSubmit(engine.GraphicsQueue, 1, &submitInfo, engine.inFlightFences[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { engine.SwapChain.GetSwapChain() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(engine.PresentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
        framebufferResized = false;
        RebuildSwapChain(engine, window);
    }
    else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    CommandBufferSubmitList.clear();
}

void Renderer::Destroy(VulkanEngine& engine)
{

   // frameBufferRenderPass.Destroy(engine);
    //WaterRenderPass.Destroy(engine);
    //gBufferRenderPass.Destroy(engine);
    //AnimationRenderer.Destroy(engine);
    pbrRenderer.Destroy(engine);
    skybox.Destory(engine);
    RayRenderer.Destory(engine);
}
