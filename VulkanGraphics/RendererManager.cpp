#include "RendererManager.h"
#include <stb_image.h>
#include "BlinnPhongPipeline.h"

RendererManager::RendererManager()
{
}

RendererManager::RendererManager(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window)
{
    interfaceRenderPass = InterfaceRenderPass(engine);

    // blinnPhongRenderer = BlinnPhongRasterRenderer(AssetManagerPtr::GetAssetPtr());
    // pbrRenderer = PBRRenderer(EnginePtr::GetEnginePtr());
    //rayTraceRenderer = RayTraceRenderer(EnginePtr::GetEnginePtr());
    //pbrRayTraceRenderer = RayTracePBRRenderer(EnginePtr::GetEnginePtr());
    //hybridRenderer = HybridRenderer(EnginePtr::GetEnginePtr());
    ////guiRenderer = GUIRenderer(EnginePtr::GetEnginePtr());
    //renderer2D = Renderer2D(EnginePtr::GetEnginePtr());



    MaterialTexture material;
    material.DiffuseMap = TextureManagerPtr::GetTextureManagerPtr()->LoadTexture2D(std::make_shared<Texture2D>(Texture2D("../texture/texture.jpg", VK_FORMAT_R8G8B8A8_SRGB)));
    auto a = MaterialManagerPtr::GetMaterialManagerPtr()->LoadMaterial("aza", material);
    MaterialManagerPtr::GetMaterialManagerPtr()->UpdateBufferIndex();

    MeshManagerPtr::GetMeshManagerPtr()->AddMesh(std::make_shared<Mesh>(Mesh(vertices, indices, a, Mesh_Draw_All)));

    BlinnRenderer = BlinnPhongRasterRenderer(engine);
}

RendererManager::~RendererManager()
{

}

void RendererManager::RebuildSwapChain(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window)
{
    int width = 0, height = 0;
    glfwGetFramebufferSize(window->GetWindowPtr(), &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(window->GetWindowPtr(), &width, &height);
        glfwWaitEvents();
    }
    EnginePtr::GetEnginePtr()->ScreenResoulation = glm::ivec2(width, height);


    vkDeviceWaitIdle(engine->Device);

    for (auto imageView : engine->SwapChain.GetSwapChainImageViews()) {
        vkDestroyImageView(engine->Device, imageView, nullptr);
    }

    vkDestroySwapchainKHR(engine->Device, engine->SwapChain.GetSwapChain(), nullptr);

   // AssetManagerPtr::GetAssetPtr()->Update();
    engine->SwapChain.RebuildSwapChain(window->GetWindowPtr(), engine->Device, engine->PhysicalDevice, engine->Surface);

    interfaceRenderPass.RebuildSwapChain();
    BlinnRenderer.RebuildSwapChain();

   //blinnPhongRenderer.RebuildSwapChain();
    //pbrRenderer.RebuildSwapChain();
    //rayTraceRenderer.RebuildSwapChain();
    //pbrRayTraceRenderer.RebuildSwapChain();
    //hybridRenderer.RebuildSwapChain();
    //renderer2D.RebuildSwapChain();
    //guiRenderer.RebuildSwapChain(engine, window);
}

void RendererManager::Update(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window, uint32_t currentImage)
{
  /*  if(engine->UpdateRendererFlag)
    {
        AssetManagerPtr::GetAssetPtr()->UpdateBufferIndex();
        RebuildSwapChain(engine, window);
        engine->UpdateRendererFlag = false;
    }*/

    AssetManagerPtr::GetAssetPtr()->Update();
    //if (EnginePtr::GetEnginePtr()->RayTraceFlag)
    //{
       //rayTraceRenderer.rayTraceRenderPass.SetUpTopLevelAccelerationStructure(engine, AssetManagerPtr::GetAssetPtr());
       // pbrRayTraceRenderer.rayTraceRenderPass.SetUpTopLevelAccelerationStructure(engine, AssetManagerPtr::GetAssetPtr());
       // hybridRenderer.rayTraceRenderPass.SetUpTopLevelAccelerationStructure(engine, AssetManagerPtr::GetAssetPtr());
   // }
}

void RendererManager::GUIUpdate(std::shared_ptr<VulkanEngine> engine)
{
    //ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    //ImGui::SliderInt("Active Renderer", &ActiveRenderer, 0, 5);
    //ImGui::SliderInt("Active Camera", &AssetManagerPtr::GetAssetPtr()->cameraManager->cameraIndex, 0, AssetManagerPtr::GetAssetPtr()->cameraManager->CameraList.size());

    //if (ActiveRenderer == 0)
    //{
    //    blinnPhongRenderer.GUIUpdate();
    //}
    //else if (ActiveRenderer == 1)
    //{
    //    pbrRenderer.GUIUpdate();
    //}
    //else if (ActiveRenderer == 2)
    //{
    //    rayTraceRenderer.GUIUpdate();
    //}
    //else if (ActiveRenderer == 3)
    //{
    //    pbrRayTraceRenderer.GUIUpdate();
    //}
    //else if (ActiveRenderer == 4)
    //{
    //    hybridRenderer.GUIUpdate();
    //}
    //else if (ActiveRenderer == 5)
    //{
    //    renderer2D.GUIUpdate();
    //}
   /* guiRenderer.GUIUpdate();*/
}

void RendererManager::Draw(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window)
{
  // vkDeviceWaitIdle(engine->Device);
    vkWaitForFences(EnginePtr::GetEnginePtr()->Device, 1, &EnginePtr::GetEnginePtr()->inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
    VkResult result = vkAcquireNextImageKHR(EnginePtr::GetEnginePtr()->Device, EnginePtr::GetEnginePtr()->SwapChain.GetSwapChain(), UINT64_MAX, EnginePtr::GetEnginePtr()->vulkanSemaphores[currentFrame].ImageAcquiredSemaphore, VK_NULL_HANDLE, &EnginePtr::GetEnginePtr()->DrawFrame);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        RebuildSwapChain(engine, window);
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    Update(engine, window, EnginePtr::GetEnginePtr()->DrawFrame);

    VkSemaphore waitSemaphores[] = { EnginePtr::GetEnginePtr()->vulkanSemaphores[currentFrame].ImageAcquiredSemaphore };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    if (EnginePtr::GetEnginePtr()->imagesInFlight[EnginePtr::GetEnginePtr()->DrawFrame] != VK_NULL_HANDLE) {
        vkWaitForFences(EnginePtr::GetEnginePtr()->Device, 1, &EnginePtr::GetEnginePtr()->imagesInFlight[EnginePtr::GetEnginePtr()->DrawFrame], VK_TRUE, UINT64_MAX);
    }
    EnginePtr::GetEnginePtr()->imagesInFlight[EnginePtr::GetEnginePtr()->DrawFrame] = EnginePtr::GetEnginePtr()->inFlightFences[currentFrame];

    VkSemaphore signalSemaphores[] = { EnginePtr::GetEnginePtr()->vulkanSemaphores[currentFrame].RenderCompleteSemaphore };

    std::vector<VkCommandBuffer> CommandBufferSubmitList;
   
    BlinnRenderer.Draw(currentFrame);
    BlinnRenderer.AddToCommandBufferSubmitList(CommandBufferSubmitList);

    interfaceRenderPass.Draw(currentFrame, EnginePtr::GetEnginePtr()->DrawFrame);
    CommandBufferSubmitList.emplace_back(interfaceRenderPass.ImGuiCommandBuffers);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.commandBufferCount = static_cast<uint32_t>(CommandBufferSubmitList.size());
    submitInfo.pCommandBuffers = CommandBufferSubmitList.data();
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences(EnginePtr::GetEnginePtr()->Device, 1, &EnginePtr::GetEnginePtr()->inFlightFences[currentFrame]);

    if (vkQueueSubmit(EnginePtr::GetEnginePtr()->GraphicsQueue, 1, &submitInfo, EnginePtr::GetEnginePtr()->inFlightFences[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { EnginePtr::GetEnginePtr()->SwapChain.GetSwapChain() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &EnginePtr::GetEnginePtr()->DrawFrame;

    result = vkQueuePresentKHR(EnginePtr::GetEnginePtr()->PresentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
        framebufferResized = false;
        RebuildSwapChain(engine, window);
    }
    else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void RendererManager::Destroy(std::shared_ptr<VulkanEngine> engine)
{
    BlinnRenderer.Destroy();

    interfaceRenderPass.Destroy();

   // blinnPhongRenderer.Destroy();
    //pbrRenderer.Destroy();
    //rayTraceRenderer.Destroy();
    //pbrRayTraceRenderer.Destroy();
    //hybridRenderer.Destroy();
    //renderer2D.Destroy();
    //guiRenderer.Destroy();
}