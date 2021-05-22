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
    interfaceRenderPass = InterfaceRenderPass(engine, window.GetWindowPtr());
    assetManager = assetManagerPTR;

    blinnPhongRenderer = BlinnPhongRasterRenderer(engine, window, assetManager);
    //pbrRenderer = PBRRenderer(engine, window, assetManager);
    //pbrRayTraceRenderer = RayTracePBRRenderer(engine, window, assetManager);
    rayTraceRenderer = RayTraceRenderer(engine, window, assetManager);
    hybridRenderer = HybridRenderer(engine, window, assetManager);
}

Renderer::~Renderer()
{

}

void Renderer::RebuildSwapChain(VulkanEngine& engine, VulkanWindow& window)
{
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

    assetManager->Update(engine);
    engine.SwapChain.RebuildSwapChain(window.GetWindowPtr(), engine.Device, engine.PhysicalDevice, engine.Surface);

    interfaceRenderPass.RebuildSwapChain(engine);
    blinnPhongRenderer.RebuildSwapChain(engine, window);
    //pbrRenderer.RebuildSwapChain(engine, window);
    //pbrRayTraceRenderer.RebuildSwapChain(engine, window);
    rayTraceRenderer.RebuildSwapChain(engine, window);
    hybridRenderer.RebuildSwapChain(engine, window);
}

void Renderer::Update(VulkanEngine& engine, VulkanWindow& window, uint32_t currentImage)
{
    if(addlightflag)
    {
        DirectionalLightBuffer dlight = DirectionalLightBuffer();
        dlight.direction = glm::vec4(0.0f);
        dlight.ambient = glm::vec4(0.2f);
        dlight.diffuse = glm::vec4(0.5f);
        dlight.specular = glm::vec4(1.0f);
        assetManager->lightManager.AddDirectionalLight(engine, dlight);
        UpdateRenderer = true;
        addlightflag = false;
    }
    if (deletelightflag)
    {
        assetManager->lightManager.DeleteDirectionalLight(engine, 0);
        UpdateRenderer = true;
        deletelightflag = false;
    }
    if (addplightflag)
    {
        PointLightBuffer dlight = PointLightBuffer();
        dlight.position = glm::vec4(0.0f);
        dlight.ambient = glm::vec4(0.2f);
        dlight.diffuse = glm::vec4(0.5f);
        dlight.specular = glm::vec4(1.0f);
        assetManager->lightManager.AddPointLight(engine, dlight);
        UpdateRenderer = true;
        addplightflag = false;
    }
    if (deleteplightflag)
    {
        assetManager->lightManager.DeletePointLight(engine, 0);
        UpdateRenderer = true;
        deleteplightflag = false;
    }
    if(UpdateRenderer)
    {
        RebuildSwapChain(engine, window);
        UpdateRenderer = false;
    }

    keyboard.Update(window.GetWindowPtr(), assetManager->ActiveCamera);
    mouse.Update(window.GetWindowPtr(), assetManager->ActiveCamera);

    assetManager->Update(engine);
    rayTraceRenderer.rayTraceRenderPass.SetUpTopLevelAccelerationStructure(engine, assetManager);
    //pbrRayTraceRenderer.rayTraceRenderPass.SetUpTopLevelAccelerationStructure(engine, assetManager);
    hybridRenderer.rayTraceRenderPass.SetUpTopLevelAccelerationStructure(engine, assetManager);
}

void Renderer::GUIUpdate(VulkanEngine& engine)
{
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Checkbox("AddLight", &addlightflag);
    ImGui::Checkbox("Deletelightflag", &deletelightflag);
    ImGui::Checkbox("AddpLight", &addplightflag);
    ImGui::Checkbox("Deleteplightflag", &deleteplightflag);
    ImGui::SliderInt("Active Renderer", &ActiveRenderer, 0, 5);
    ImGui::SliderInt("Active Camera", &assetManager->cameraIndex, 0, assetManager->CameraList.size());
    if (ActiveRenderer == 0)
    {
        blinnPhongRenderer.GUIUpdate(engine);
    }
    //else if (ActiveRenderer == 1)
    //{
    //    pbrRenderer.GUIUpdate(engine);
    //}
    else if (ActiveRenderer == 2)
    {
        rayTraceRenderer.GUIUpdate(engine);
    }
    //else if (ActiveRenderer == 3)
    //{
    //    pbrRayTraceRenderer.GUIUpdate(engine);
    //}
    else if (ActiveRenderer == 4)
    {
        hybridRenderer.GUIUpdate(engine);
    }
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

    VkSemaphore waitSemaphores[] = { engine.vulkanSemaphores[currentFrame].ImageAcquiredSemaphore };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    for (auto& model : assetManager->modelManager.ModelList)
    {
        model->SubmitToCommandBuffer(engine, CommandBufferSubmitList, imageIndex);
    }

    if (ActiveRenderer == 0)
    {
        blinnPhongRenderer.Draw(engine, window, imageIndex);
        blinnPhongRenderer.AddToCommandBufferSubmitList(CommandBufferSubmitList);
    }
    //else if (ActiveRenderer == 1)
    //{
    //    pbrRenderer.Draw(engine, window, imageIndex);
    //    pbrRenderer.AddToCommandBufferSubmitList(CommandBufferSubmitList);
    //}
    else if (ActiveRenderer == 2)
    {
        rayTraceRenderer.Draw(engine, window, imageIndex);
        rayTraceRenderer.AddToCommandBufferSubmitList(CommandBufferSubmitList);
    }
    //else if (ActiveRenderer == 3)
    //{
    //    pbrRayTraceRenderer.Draw(engine, window, imageIndex);
    //    pbrRayTraceRenderer.AddToCommandBufferSubmitList(CommandBufferSubmitList);
    //}
    else if (ActiveRenderer == 4)
    {
        hybridRenderer.Draw(engine, window, imageIndex);
        hybridRenderer.AddToCommandBufferSubmitList(CommandBufferSubmitList);
    }

    interfaceRenderPass.Draw(engine, imageIndex);
    CommandBufferSubmitList.emplace_back(interfaceRenderPass.ImGuiCommandBuffers);

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
    interfaceRenderPass.Destroy(engine);
    blinnPhongRenderer.Destroy(engine);
    hybridRenderer.Destroy(engine);
    //pbrRenderer.Destroy(engine);
    rayTraceRenderer.Destroy(engine);
    //pbrRayTraceRenderer.Destroy(engine);
}
