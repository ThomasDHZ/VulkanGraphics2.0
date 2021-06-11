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
    pbrRenderer = PBRRenderer(engine, window, assetManager);
    rayTraceRenderer = RayTraceRenderer(engine, window, assetManager);
    pbrRayTraceRenderer = RayTracePBRRenderer(engine, window, assetManager);
    hybridRenderer = HybridRenderer(engine, window, assetManager);
    renderer2D = Renderer2D(engine, window, assetManager);
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
    pbrRenderer.RebuildSwapChain(engine, window);
    rayTraceRenderer.RebuildSwapChain(engine, window);
    pbrRayTraceRenderer.RebuildSwapChain(engine, window);
    hybridRenderer.RebuildSwapChain(engine, window);
    renderer2D.RebuildSwapChain(engine, window);
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
        assetManager->lightManager.AddDirectionalLight(engine, assetManager->cameraManager, dlight);
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

    keyboard.Update(window.GetWindowPtr(), assetManager->cameraManager.ActiveCamera);
    mouse.Update(window.GetWindowPtr(), assetManager->cameraManager.ActiveCamera);

    assetManager->Update(engine);
    rayTraceRenderer.rayTraceRenderPass.SetUpTopLevelAccelerationStructure(engine, assetManager);
    pbrRayTraceRenderer.rayTraceRenderPass.SetUpTopLevelAccelerationStructure(engine, assetManager);
    hybridRenderer.rayTraceRenderPass.SetUpTopLevelAccelerationStructure(engine, assetManager);
}

void Renderer::GUIUpdate(VulkanEngine& engine)
{
    ImGui::SliderFloat3("Pos", &assetManager->SceneData->UniformDataInfo.dlight.direction.x, -1.0f, 1.0f);
    ImGui::SliderFloat3("Diffuse", &assetManager->SceneData->UniformDataInfo.dlight.diffuse.x, 0.0f, 1.0f);


    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Checkbox("AddLight", &addlightflag);
    ImGui::Checkbox("Deletelightflag", &deletelightflag);
    ImGui::Checkbox("AddpLight", &addplightflag);
    ImGui::Checkbox("Deleteplightflag", &deleteplightflag);
    ImGui::SliderInt("Active Renderer", &ActiveRenderer, 0, 5);
    ImGui::SliderInt("Active Camera", &assetManager->cameraManager.cameraIndex, 0, assetManager->cameraManager.CameraList.size());
    
 /*   ImGui::SliderFloat("adsf", &assetManager->materialManager.MaterialList[0]->materialTexture.Reflectivness, 0.0f, 1.0f);
    ImGui::SliderFloat("adsf2", &assetManager->materialManager.MaterialList[1]->materialTexture.Reflectivness, 0.0f, 1.0f);
    ImGui::SliderFloat("adsf3", &assetManager->materialManager.MaterialList[2]->materialTexture.Reflectivness, 0.0f, 1.0f);
    ImGui::SliderFloat("adsf4", &assetManager->materialManager.MaterialList[3]->materialTexture.Reflectivness, 0.0f, 1.0f);*/
    //ImGui::SliderFloat("adsf5", &assetManager->materialManager.MaterialList[4]->materialTexture.Reflectivness, 0.0f, 1.0f);
    //ImGui::SliderFloat("adsf6", &assetManager->materialManager.MaterialList[5]->materialTexture.Reflectivness, 0.0f, 1.0f);
    //ImGui::SliderFloat("adsf7", &assetManager->materialManager.MaterialList[6]->materialTexture.Reflectivness, 0.0f, 1.0f);
    //ImGui::SliderFloat("adsf8", &assetManager->materialManager.MaterialList[7]->materialTexture.Reflectivness, 0.0f, 1.0f);
    //ImGui::SliderFloat("adsf9", &assetManager->materialManager.MaterialList[8]->materialTexture.Reflectivness, 0.0f, 1.0f);
    //ImGui::SliderFloat("adsf10", &assetManager->materialManager.MaterialList[9]->materialTexture.Reflectivness, 0.0f, 1.0f);
    //ImGui::SliderFloat("adsf11", &assetManager->materialManager.MaterialList[10]->materialTexture.Reflectivness, 0.0f, 1.0f);
    //ImGui::SliderFloat("adsf12", &assetManager->materialManager.MaterialList[11]->materialTexture.Reflectivness, 0.0f, 1.0f);
    //ImGui::SliderFloat("adsf13", &assetManager->materialManager.MaterialList[12]->materialTexture.Reflectivness, 0.0f, 1.0f);
    //ImGui::SliderFloat("adsf14", &assetManager->materialManager.MaterialList[13]->materialTexture.Reflectivness, 0.0f, 1.0f);
    //ImGui::SliderFloat("adsf15", &assetManager->materialManager.MaterialList[14]->materialTexture.Reflectivness, 0.0f, 1.0f);
    //ImGui::SliderFloat("adsf16", &assetManager->materialManager.MaterialList[15]->materialTexture.Reflectivness, 0.0f, 1.0f);
    //ImGui::SliderFloat("adsf17", &assetManager->materialManager.MaterialList[16]->materialTexture.Reflectivness, 0.0f, 1.0f);


    if (ActiveRenderer == 0)
    {
        blinnPhongRenderer.GUIUpdate(engine);
    }
    else if (ActiveRenderer == 1)
    {
        pbrRenderer.GUIUpdate(engine);
    }
    else if (ActiveRenderer == 2)
    {
        rayTraceRenderer.GUIUpdate(engine);
    }
    else if (ActiveRenderer == 3)
    {
        pbrRayTraceRenderer.GUIUpdate(engine);
    }
    else if (ActiveRenderer == 4)
    {
        hybridRenderer.GUIUpdate(engine);
    }
    else if (ActiveRenderer == 5)
    {
        renderer2D.GUIUpdate(engine);
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
    else if (ActiveRenderer == 1)
    {
        pbrRenderer.Draw(engine, window, imageIndex);
        pbrRenderer.AddToCommandBufferSubmitList(CommandBufferSubmitList);
    }
    else if (ActiveRenderer == 2)
    {
        rayTraceRenderer.Draw(engine, window, imageIndex);
        rayTraceRenderer.AddToCommandBufferSubmitList(CommandBufferSubmitList);
    }
    else if (ActiveRenderer == 3)
    {
        pbrRayTraceRenderer.Draw(engine, window, imageIndex);
        pbrRayTraceRenderer.AddToCommandBufferSubmitList(CommandBufferSubmitList);
    }
    else if (ActiveRenderer == 4)
    {
        hybridRenderer.Draw(engine, window, imageIndex);
        hybridRenderer.AddToCommandBufferSubmitList(CommandBufferSubmitList);
    }
    else if (ActiveRenderer == 5)
    {
        renderer2D.Draw(engine, window, imageIndex);
        renderer2D.AddToCommandBufferSubmitList(CommandBufferSubmitList);
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
    pbrRenderer.Destroy(engine);
    rayTraceRenderer.Destroy(engine);
    pbrRayTraceRenderer.Destroy(engine);
    hybridRenderer.Destroy(engine);
}
