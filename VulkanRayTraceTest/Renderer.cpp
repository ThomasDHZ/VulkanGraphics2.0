#include "Renderer.h"
#include <stb_image.h>
#include "Sprite.h"
#include "MegaMan.h"
#include "Mario.h"
#include "TerrainMesh.h"

Renderer::Renderer()
{
}

Renderer::Renderer(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window, std::shared_ptr<AssetManager> assetManagerPTR)
{
    interfaceRenderPass = InterfaceRenderPass(engine);

     blinnPhongRenderer = BlinnPhongRasterRenderer(AssetManagerPtr::GetAssetPtr());
     pbrRenderer = PBRRenderer(EnginePtr::GetEnginePtr());
    rayTraceRenderer = RayTraceRenderer(EnginePtr::GetEnginePtr());
    pbrRayTraceRenderer = RayTracePBRRenderer(EnginePtr::GetEnginePtr());
    hybridRenderer = HybridRenderer(EnginePtr::GetEnginePtr());
    //guiRenderer = GUIRenderer(EnginePtr::GetEnginePtr());
    renderer2D = Renderer2D(EnginePtr::GetEnginePtr());
}

Renderer::~Renderer()
{

}

void Renderer::RebuildSwapChain(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window)
{
    int width = 0, height = 0;
    glfwGetFramebufferSize(window->GetWindowPtr(), &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(window->GetWindowPtr(), &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(engine->Device);

    for (auto imageView : engine->SwapChain.GetSwapChainImageViews()) {
        vkDestroyImageView(engine->Device, imageView, nullptr);
    }

    vkDestroySwapchainKHR(engine->Device, engine->SwapChain.GetSwapChain(), nullptr);

    AssetManagerPtr::GetAssetPtr()->Update(RayTraceFlag);
    engine->SwapChain.RebuildSwapChain(window->GetWindowPtr(), engine->Device, engine->PhysicalDevice, engine->Surface);

    interfaceRenderPass.RebuildSwapChain();
    blinnPhongRenderer.RebuildSwapChain();
    pbrRenderer.RebuildSwapChain();
    rayTraceRenderer.RebuildSwapChain();
    pbrRayTraceRenderer.RebuildSwapChain();
    hybridRenderer.RebuildSwapChain();
    renderer2D.RebuildSwapChain();
    //guiRenderer.RebuildSwapChain(engine, window);
}

void Renderer::Update(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window, uint32_t currentImage)
{
    if(engine->UpdateRendererFlag)
    {
        RebuildSwapChain(engine, window);
        engine->UpdateRendererFlag = false;
    }

    AssetManagerPtr::GetAssetPtr()->Update(RayTraceFlag);
    if (RayTraceFlag)
    {
       rayTraceRenderer.rayTraceRenderPass.SetUpTopLevelAccelerationStructure(engine, AssetManagerPtr::GetAssetPtr());
        pbrRayTraceRenderer.rayTraceRenderPass.SetUpTopLevelAccelerationStructure(engine, AssetManagerPtr::GetAssetPtr());
        hybridRenderer.rayTraceRenderPass.SetUpTopLevelAccelerationStructure(engine, AssetManagerPtr::GetAssetPtr());
    }
}

void Renderer::GUIUpdate(std::shared_ptr<VulkanEngine> engine)
{
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::SliderInt("Active Renderer", &ActiveRenderer, 0, 5);
    ImGui::SliderInt("Active Camera", &AssetManagerPtr::GetAssetPtr()->cameraManager->cameraIndex, 0, AssetManagerPtr::GetAssetPtr()->cameraManager->CameraList.size());

    if (ActiveRenderer == 0)
    {
        blinnPhongRenderer.GUIUpdate();
    }
    else if (ActiveRenderer == 1)
    {
        pbrRenderer.GUIUpdate();
    }
    else if (ActiveRenderer == 2)
    {
        rayTraceRenderer.GUIUpdate();
    }
    else if (ActiveRenderer == 3)
    {
        pbrRayTraceRenderer.GUIUpdate();
    }
    else if (ActiveRenderer == 4)
    {
        hybridRenderer.GUIUpdate();
    }
    else if (ActiveRenderer == 5)
    {
        renderer2D.GUIUpdate();
    }
   /* guiRenderer.GUIUpdate();*/
}

void Renderer::Draw(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window)
{
    vkWaitForFences(engine->Device, 1, &engine->inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    VkResult result = vkAcquireNextImageKHR(engine->Device, engine->SwapChain.GetSwapChain(), UINT64_MAX, engine->vulkanSemaphores[currentFrame].ImageAcquiredSemaphore, VK_NULL_HANDLE, &EnginePtr::GetEnginePtr()->DrawFrame);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        RebuildSwapChain(engine, window);
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    Update(engine, window, EnginePtr::GetEnginePtr()->DrawFrame);

    if (engine->imagesInFlight[EnginePtr::GetEnginePtr()->DrawFrame] != VK_NULL_HANDLE) {
        vkWaitForFences(engine->Device, 1, &engine->imagesInFlight[EnginePtr::GetEnginePtr()->DrawFrame], VK_TRUE, UINT64_MAX);
    }

    engine->imagesInFlight[EnginePtr::GetEnginePtr()->DrawFrame] = engine->inFlightFences[currentFrame];

    VkSemaphore waitSemaphores[] = { engine->vulkanSemaphores[currentFrame].ImageAcquiredSemaphore };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    AssetManagerPtr::GetAssetPtr()->ObjManager->SubmitAnimationToCommandBuffer(CommandBufferSubmitList, EnginePtr::GetEnginePtr()->DrawFrame);
    if (ActiveRenderer == 0)
    {
        RayTraceFlag = false;
        blinnPhongRenderer.Draw();
        blinnPhongRenderer.AddToCommandBufferSubmitList(CommandBufferSubmitList);
    }
    else if (ActiveRenderer == 1)
    {
        RayTraceFlag = false;
        pbrRenderer.Draw();
        pbrRenderer.AddToCommandBufferSubmitList(CommandBufferSubmitList);
    }
    else if (ActiveRenderer == 2)
    {
        RayTraceFlag = true;
        rayTraceRenderer.Draw();
        rayTraceRenderer.AddToCommandBufferSubmitList(CommandBufferSubmitList);
    }
    else if (ActiveRenderer == 3)
    {
        RayTraceFlag = true;
        pbrRayTraceRenderer.Draw();
        pbrRayTraceRenderer.AddToCommandBufferSubmitList(CommandBufferSubmitList);
    }
    else if (ActiveRenderer == 4)
    {
        RayTraceFlag = true;
        hybridRenderer.Draw();
        hybridRenderer.AddToCommandBufferSubmitList(CommandBufferSubmitList);
    }
    else if (ActiveRenderer == 5)
    {
        RayTraceFlag = false;
        renderer2D.Draw();
        renderer2D.AddToCommandBufferSubmitList(CommandBufferSubmitList);
    }

    //guiRenderer.Draw();
    //guiRenderer.AddToCommandBufferSubmitList(CommandBufferSubmitList);
    interfaceRenderPass.Draw();
    CommandBufferSubmitList.emplace_back(interfaceRenderPass.ImGuiCommandBuffers);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = static_cast<uint32_t>(CommandBufferSubmitList.size());
    submitInfo.pCommandBuffers = CommandBufferSubmitList.data();

    VkSemaphore signalSemaphores[] = { engine->vulkanSemaphores[currentFrame].RenderCompleteSemaphore };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences(engine->Device, 1, &engine->inFlightFences[currentFrame]);

    if (vkQueueSubmit(engine->GraphicsQueue, 1, &submitInfo, engine->inFlightFences[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { engine->SwapChain.GetSwapChain() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &EnginePtr::GetEnginePtr()->DrawFrame;

    result = vkQueuePresentKHR(engine->PresentQueue, &presentInfo);

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

void Renderer::Destroy(std::shared_ptr<VulkanEngine> engine)
{
    interfaceRenderPass.Destroy();
    blinnPhongRenderer.Destroy();
    pbrRenderer.Destroy();
    rayTraceRenderer.Destroy();
    pbrRayTraceRenderer.Destroy();
    hybridRenderer.Destroy();
    renderer2D.Destroy();
    //guiRenderer.Destroy();
}
