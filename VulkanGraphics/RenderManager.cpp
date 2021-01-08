#include "RenderManager.h"
#include "ImGui/imgui_impl_vulkan.h"
#include <random>

RenderManager::RenderManager()
{
}

RenderManager::RenderManager(VulkanEngine& engine, std::shared_ptr<TextureManager> textureManager, GLFWwindow* window)
{
    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(3);
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(3);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = 3;

    if (vkCreateDescriptorPool(engine.Device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
   

    VkCommandPoolCreateInfo poolInfo2{};
    poolInfo2.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo2.queueFamilyIndex = engine.GraphicsFamily;

    if (vkCreateCommandPool(engine.Device, &poolInfo2, nullptr, &commandPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics command pool!");
    }

    uint32_t width = engine.SwapChain.GetSwapChainResolution().width;
    uint32_t height = engine.SwapChain.GetSwapChainResolution().height;
    rayTracer = RayTraceRenderer(engine.Device, engine.PhysicalDevice, commandPool, engine.GraphicsQueue, descriptorPool, width, height, 3, engine.SwapChain.SwapChainImages);

    interfaceRenderPass = InterfaceRenderPass(engine, window);
    mainRenderPass = MainRenderPass(engine);
    sceneRenderPass = SceneRenderPass(engine);
    gBufferRenderPass = GBufferRenderPass(engine);
    frameBufferRenderPass = FrameBufferRenderPass(engine);
    textureRenderPass = SceneToTextureRendererPass(engine);
    bloomRenderPass = BloomRenderPass(engine, textureManager, sceneRenderPass.BloomTexture);
    shadowRenderPass = ShadowRenderPass(engine);

    //SSAOFrameBuffer = DeferredFrameBufferMesh(engine, gBufferRenderPass.GPositionTexture, gBufferRenderPass.GNormalTexture, gBufferRenderPass.ssaoPipeline->ShaderPipelineDescriptorLayout);
    //SSAOBlurframeBuffer = FrameBufferMesh(engine, gBufferRenderPass.SSAOTexture, frameBufferRenderPass.frameBufferPipeline->ShaderPipelineDescriptorLayout);
    frameBuffer = FrameBufferMesh(engine, textureManager, sceneRenderPass.ColorTexture, frameBufferRenderPass.frameBufferPipeline->ShaderPipelineDescriptorLayout, bloomRenderPass.OutputBloomImage());
}

RenderManager::~RenderManager()
{
}

void RenderManager::ResizeWindowUpdate(VulkanEngine& engine, GLFWwindow* window, std::shared_ptr<TextureManager> textureManager, std::shared_ptr<Camera> camera, std::vector<Model>& ModelList, SkyBoxMesh& skybox, LightManager& lightmanager, std::vector<std::shared_ptr<Object2D>>& SpriteList, std::vector<Mesh>& MeshList)
{
    int width = 0, height = 0;
    glfwGetFramebufferSize(window, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(window, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(engine.Device);

    for (auto imageView : engine.SwapChain.GetSwapChainImageViews()) {
        vkDestroyImageView(engine.Device, imageView, nullptr);
    }

    vkDestroySwapchainKHR(engine.Device, engine.SwapChain.GetSwapChain(), nullptr);

    engine.SwapChain.UpdateSwapChain(window, engine.Device, engine.PhysicalDevice, engine.Surface);

    /// mainRenderPass.UpdateSwapChain(engine);
    shadowRenderPass.UpdateSwapChain(engine);
     sceneRenderPass.UpdateSwapChain(engine);
  //  gBufferRenderPass.UpdateSwapChain(engine);
  //  frameBufferRenderPass.UpdateSwapChain(engine);
    bloomRenderPass.UpdateSwapChain(engine, textureManager, sceneRenderPass.BloomTexture);
    interfaceRenderPass.UpdateSwapChain(engine);

    //SSAOFrameBuffer.UpdateSwapChain(engine, gBufferRenderPass.GPositionTexture, gBufferRenderPass.GNormalTexture, gBufferRenderPass.ssaoPipeline->ShaderPipelineDescriptorLayout);
   // SSAOBlurframeBuffer = FrameBufferMesh(engine, gBufferRenderPass.SSAOTexture, frameBufferRenderPass.frameBufferPipeline->ShaderPipelineDescriptorLayout);
    frameBuffer.UpdateSwapChain(engine, sceneRenderPass.ColorTexture, frameBufferRenderPass.frameBufferPipeline->ShaderPipelineDescriptorLayout);

    CMDBuffer(engine, camera, ModelList, skybox, lightmanager, SpriteList, MeshList);
}

void RenderManager::FrameDebug()
{
    ImGui::Begin("VRAM");
    sceneRenderPass.FrameDebug();
    ImGui::End();
}

void RenderManager::CMDBuffer(VulkanEngine& engine, std::shared_ptr<Camera> camera, std::vector<Model>& ModelList, SkyBoxMesh& skybox, LightManager& lightmanager, std::vector<std::shared_ptr<Object2D>>& SpriteList, std::vector<Mesh>& MeshList)
{
    commandBuffers.resize(mainRenderPass.SwapChainFramebuffers.size());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = engine.GetRenderCommandPool();
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

    if (vkAllocateCommandBuffers(engine.Device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }

    for (size_t i = 0; i < commandBuffers.size(); i++) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }
     //   MainRenderCMDBuffer(engine, ModelList, skybox, i, lightmanager, SpriteList);
        ShadowRenderCMDBuffer(engine, ModelList, i);
        SceneRenderCMDBuffer(engine, ModelList, skybox, i, lightmanager, SpriteList, MeshList);
      //  GBufferRenderCMDBuffer(engine, ModelList, skybox, i);
       // SSAORenderCMDBuffer(engine, camera, i);
        TextureRenderCMDBuffer(engine, i, SpriteList);
        bloomRenderPass.Draw(engine, commandBuffers, i);
        FrameBufferRenderCMDBuffer(engine, i);
        if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }
}

float lerp(float a, float b, float f)
{
    return a + f * (b - a);
}

void RenderManager::RendererUpdate(VulkanEngine& engine, std::shared_ptr<Camera> camera)
{
    SSAOConfig config = {};
    std::uniform_real_distribution<float> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
    std::default_random_engine generator;
    for (unsigned int i = 0; i < 64; ++i)
    {
        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        float scale = float(i) / 64.0;

        // scale samples s.t. they're more aligned to center of kernel
        scale = lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        config.Samples[i] = sample;
    }
    config.Projection;
    config.KernelSize = 64;
    config.Radius = 0.5f;
    config.Bias = 0.025f;
    config.Projection = camera->GetProjectionMatrix();
    config.Projection[1][1] *= -1;

    //  SSAOFrameBuffer.Update(engine, config);
    frameBuffer.Update(engine);
}

void RenderManager::UpdateCommandBuffer(VulkanEngine& engine, std::shared_ptr<Camera> camera, std::vector<Model>& ModelList, SkyBoxMesh& skybox, LightManager& lightmanager, std::vector<std::shared_ptr<Object2D>>& SpriteList, std::vector<Mesh>& MeshList)
{
    CMDBuffer(engine, camera, ModelList, skybox, lightmanager, SpriteList, MeshList);
}

void RenderManager::Draw(VulkanEngine& engine, GLFWwindow* window, std::shared_ptr<TextureManager> textureManager, std::shared_ptr<PerspectiveCamera> camera, std::vector<Model>& ModelList, SkyBoxMesh& skybox, LightManager& lightmanager, std::vector<std::shared_ptr<Object2D>>& SpriteList, std::vector<Mesh>& MeshList)
{
    vkWaitForFences(engine.Device, 1, &engine.inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    VkResult result = vkAcquireNextImageKHR(engine.Device, engine.SwapChain.GetSwapChain(), UINT64_MAX, engine.vulkanSemaphores[currentFrame].ImageAcquiredSemaphore, VK_NULL_HANDLE, &engine.DrawFrame);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        ResizeWindowUpdate(engine, window, textureManager, camera, ModelList, skybox, lightmanager, SpriteList, MeshList);
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    if (engine.imagesInFlight[engine.DrawFrame] != VK_NULL_HANDLE) {
        vkWaitForFences(engine.Device, 1, &engine.imagesInFlight[engine.DrawFrame], VK_TRUE, UINT64_MAX);
    }
    engine.imagesInFlight[engine.DrawFrame] = engine.inFlightFences[currentFrame];

    rayTracer.updateUniformBuffers();
    interfaceRenderPass.Draw(engine);

    std::array<VkCommandBuffer, 2> submitCommandBuffers =
    { rayTracer.drawCmdBuffers[engine.DrawFrame], interfaceRenderPass.ImGuiCommandBuffers[engine.DrawFrame] };

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { engine.vulkanSemaphores[currentFrame].ImageAcquiredSemaphore };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = static_cast<uint32_t>(submitCommandBuffers.size());
    submitInfo.pCommandBuffers = submitCommandBuffers.data();

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

    presentInfo.pImageIndices = &engine.DrawFrame;

    result = vkQueuePresentKHR(engine.PresentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
        framebufferResized = false;
        ResizeWindowUpdate(engine, window, textureManager, camera, ModelList, skybox, lightmanager, SpriteList, MeshList);
    }
    else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void RenderManager::Destroy(VulkanEngine& engine)
{
    //SSAOFrameBuffer.Destory(engine);
    //SSAOBlurframeBuffer.Destroy(engine);
    frameBuffer.Destory(engine);

    interfaceRenderPass.Destroy(engine);
    mainRenderPass.Destroy(engine);
    sceneRenderPass.Destroy(engine);
    gBufferRenderPass.Destroy(engine);
    frameBufferRenderPass.Destroy(engine);
    textureRenderPass.Destroy(engine);
    bloomRenderPass.Destory(engine);
    shadowRenderPass.Destroy(engine);
}

void RenderManager::MainRenderCMDBuffer(VulkanEngine& engine, std::vector<Model>& ModelList, SkyBoxMesh& skybox, int SwapBufferImageIndex, LightManager& lightmanager, std::vector<std::shared_ptr<Object2D>>& SpriteList)
{
    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
    clearValues[1].depthStencil = { 1.0f, 0 };

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = mainRenderPass.GetRenderPass();
    renderPassInfo.framebuffer = mainRenderPass.SwapChainFramebuffers[SwapBufferImageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = engine.SwapChain.GetSwapChainResolution();
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffers[SwapBufferImageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    skybox.Draw(commandBuffers[SwapBufferImageIndex], mainRenderPass.skyBoxPipeline, SwapBufferImageIndex);
    for (auto sprite : SpriteList)
    {
        sprite->Draw(commandBuffers[SwapBufferImageIndex], mainRenderPass.forwardRenderering2DPipeline, SwapBufferImageIndex);
    }
    for (auto model : ModelList)
    {
        if(model.GetRenderFlags() & RenderDrawFlags::RenderWireFrame)
        {
            model.Draw(commandBuffers[SwapBufferImageIndex], mainRenderPass.wireFrameRendereringPipeline, SwapBufferImageIndex);
        }
        if(model.GetRenderFlags() & RenderDrawFlags::RenderWireFrameAnimated)
        {
            model.Draw(commandBuffers[SwapBufferImageIndex], mainRenderPass.AnimatedWireFramedRendereringPipeline, SwapBufferImageIndex);
        }
        if(model.GetRenderFlags() & RenderDrawFlags::RenderNormally)
        {
            model.Draw(commandBuffers[SwapBufferImageIndex], mainRenderPass.forwardRendereringPipeline, SwapBufferImageIndex);
        }
        if(model.GetRenderFlags() & RenderDrawFlags::RenderAnimated)
        {
            model.Draw(commandBuffers[SwapBufferImageIndex], mainRenderPass.AnimatedForwardRendereringPipeline, SwapBufferImageIndex);
        }
    }
    for (auto model : lightmanager.PointLightList)
    {
        model->Draw(commandBuffers[SwapBufferImageIndex], mainRenderPass.debugLightRenderingPipeline, SwapBufferImageIndex);
    }
    vkCmdEndRenderPass(commandBuffers[SwapBufferImageIndex]);
}

void RenderManager::SceneRenderCMDBuffer(VulkanEngine& engine, std::vector<Model>& ModelList, SkyBoxMesh& skybox, int SwapBufferImageIndex, LightManager& lightmanager, std::vector<std::shared_ptr<Object2D>>& SpriteList, std::vector<Mesh>& MeshList)
{
    sceneRenderPass.Draw(engine, commandBuffers, SwapBufferImageIndex, ModelList, skybox, lightmanager, SpriteList, MeshList);
}

void RenderManager::GBufferRenderCMDBuffer(VulkanEngine& engine, std::vector<Model>& ModelList, SkyBoxMesh& skybox, int SwapBufferImageIndex)
{
    std::array<VkClearValue, 5> clearValues{};
    clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
    clearValues[1].color = { 0.0f, 0.0f, 0.0f, 1.0f };
    clearValues[2].color = { 0.0f, 0.0f, 0.0f, 1.0f };
    clearValues[3].color = { 0.0f, 0.0f, 0.0f, 1.0f };
    clearValues[4].depthStencil = { 1.0f, 0 };

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = gBufferRenderPass.GetRenderPass();
    renderPassInfo.framebuffer = gBufferRenderPass.SwapChainFramebuffers[SwapBufferImageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = engine.SwapChain.GetSwapChainResolution();
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffers[SwapBufferImageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    for (auto model : ModelList)
    {
        if (model.GetRenderFlags() & RenderDrawFlags::RenderNormally)
        {
            model.Draw(commandBuffers[SwapBufferImageIndex], gBufferRenderPass.gBufferPipeline, SwapBufferImageIndex);
        }
        else if (model.GetRenderFlags() & RenderDrawFlags::RenderAnimated)
        {
            model.Draw(commandBuffers[SwapBufferImageIndex], gBufferRenderPass.AnimatedGBufferPipeline, SwapBufferImageIndex);
        }
    }
    vkCmdEndRenderPass(commandBuffers[SwapBufferImageIndex]);
}

void RenderManager::SSAORenderCMDBuffer(VulkanEngine& engine, std::shared_ptr<Camera> camera, int SwapBufferImageIndex)
{
    {
        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
        clearValues[1].depthStencil = { 1.0f, 0 };

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = gBufferRenderPass.SSAORenderPass;
        renderPassInfo.framebuffer = gBufferRenderPass.SSAOSwapChainFramebuffers[SwapBufferImageIndex];
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = engine.SwapChain.GetSwapChainResolution();
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffers[SwapBufferImageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        SSAOFrameBuffer.Draw(commandBuffers[SwapBufferImageIndex], gBufferRenderPass.ssaoPipeline, SwapBufferImageIndex);
        vkCmdEndRenderPass(commandBuffers[SwapBufferImageIndex]);
    }
    //{
    //    std::array<VkClearValue, 2> clearValues{};
    //    clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
    //    clearValues[1].depthStencil = { 1.0f, 0 };

    //    VkRenderPassBeginInfo renderPassInfo{};
    //    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    //    renderPassInfo.renderPass = gBufferRenderPass.SSAOBlurRenderPass;
    //    renderPassInfo.framebuffer = gBufferRenderPass.SSAOBlurSwapChainFramebuffers[SwapBufferImageIndex];
    //    renderPassInfo.renderArea.offset = { 0, 0 };
    //    renderPassInfo.renderArea.extent = engine.SwapChain.GetSwapChainResolution();
    //    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    //    renderPassInfo.pClearValues = clearValues.data();

    //    vkCmdBeginRenderPass(commandBuffers[SwapBufferImageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    //    SSAOBlurframeBuffer.Draw(commandBuffers[SwapBufferImageIndex], gBufferRenderPass.ssaoBlurPipeline, SwapBufferImageIndex);
    //    vkCmdEndRenderPass(commandBuffers[SwapBufferImageIndex]);
    //}
}

void RenderManager::FrameBufferRenderCMDBuffer(VulkanEngine& engine, int SwapBufferImageIndex)
{
    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
    clearValues[1].depthStencil = { 1.0f, 0 };

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = frameBufferRenderPass.GetRenderPass();
    renderPassInfo.framebuffer = frameBufferRenderPass.SwapChainFramebuffers[SwapBufferImageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = engine.SwapChain.GetSwapChainResolution();
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffers[SwapBufferImageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    frameBuffer.Draw(commandBuffers[SwapBufferImageIndex], frameBufferRenderPass.frameBufferPipeline, SwapBufferImageIndex);
    vkCmdEndRenderPass(commandBuffers[SwapBufferImageIndex]);
}

void RenderManager::TextureRenderCMDBuffer(VulkanEngine& engine, int SwapBufferImageIndex, std::vector<std::shared_ptr<Object2D>>& SpriteList)
{
    textureRenderPass.Draw(engine, commandBuffers, SwapBufferImageIndex, SpriteList);
}

void RenderManager::ShadowRenderCMDBuffer(VulkanEngine& engine, std::vector<Model>& ModelList, int SwapBufferImageIndex)
{
    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
    clearValues[1].depthStencil = { 1.0f, 0 };

    VkRenderPassBeginInfo renderPassInfo2{};
    renderPassInfo2.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo2.renderPass = shadowRenderPass.GetRenderPass();
    renderPassInfo2.framebuffer = shadowRenderPass.SwapChainFramebuffers[SwapBufferImageIndex];
    renderPassInfo2.renderArea.offset = { 0, 0 };
    renderPassInfo2.renderArea.extent = engine.SwapChain.GetSwapChainResolution();
    renderPassInfo2.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo2.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffers[SwapBufferImageIndex], &renderPassInfo2, VK_SUBPASS_CONTENTS_INLINE);
    for (auto model : ModelList)
    {
        if (model.GetRenderFlags() & RenderDrawFlags::RenderShadow)
        {
            model.Draw(commandBuffers[SwapBufferImageIndex], shadowRenderPass.shadowRendereringPipeline, SwapBufferImageIndex);
        }
        else if (model.GetRenderFlags() & RenderDrawFlags::RenderShadowAnimated)
        {
           model.Draw(commandBuffers[SwapBufferImageIndex], shadowRenderPass.AnimatedShadowRendereringPipeline, SwapBufferImageIndex);
        }
    }
    vkCmdEndRenderPass(commandBuffers[SwapBufferImageIndex]);
}
