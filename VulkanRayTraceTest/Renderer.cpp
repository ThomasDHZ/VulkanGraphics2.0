#include "Renderer.h"

Renderer::Renderer()
{
}

Renderer::Renderer(VulkanEngine& engine, VulkanWindow& window, TextureManager& textureManager, std::vector<RayTraceModel>& ModelList)
{

    SceneData = std::make_shared<SceneDataStruct>(SceneDataStruct(engine));

    SetUpDescriptorLayout(engine, textureManager);
    RenderPass = MainRenderPass(engine, descriptorSetLayout);
    //frameBufferRenderPass = FrameBufferRenderPass(engine, textureManager.GetTexture(3));
    RayRenderer = RayTraceRenderer(engine, textureManager, ModelList);
    interfaceRenderPass = InterfaceRenderPass(engine.Device, engine.Instance, engine.PhysicalDevice, engine.GraphicsQueue, window.GetWindowPtr(), engine.SwapChain.SwapChainImageViews, engine.SwapChain.SwapChainResolution);
    SetUpDescriptorPool(engine);
    SetUpDescriptorSets(engine, textureManager, ModelList);
    SetUpCommandBuffers(engine, ModelList);

    camera = std::make_shared<PerspectiveCamera>(glm::vec2(engine.SwapChain.SwapChainResolution.width, engine.SwapChain.SwapChainResolution.height), glm::vec3(0.0f, 0.0f, 5.0f));

    SceneData->SceneData.dlight.direction = glm::vec4(28.572f, 1000.0f, 771.429f, 0.0f);
    SceneData->SceneData.dlight.ambient = glm::vec4(0.2f);
    SceneData->SceneData.dlight.diffuse = glm::vec4(0.5f);
    SceneData->SceneData.dlight.specular = glm::vec4(1.0f);

    SceneData->SceneData.plight.position = glm::vec4(0.0f);
    SceneData->SceneData.plight.ambient = glm::vec4(0.2f);
    SceneData->SceneData.plight.diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 0.0f);
    SceneData->SceneData.plight.specular = glm::vec4(1.0f);
}

Renderer::~Renderer()
{

}

void Renderer::SetUpDescriptorLayout(VulkanEngine& engine, TextureManager& textureManager)
{
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.pImmutableSamplers = nullptr;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutBinding MaterialLayoutBinding{};
    MaterialLayoutBinding.binding = 5;
    MaterialLayoutBinding.descriptorCount = 1;
    MaterialLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    MaterialLayoutBinding.pImmutableSamplers = nullptr;
    MaterialLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = 6;
    samplerLayoutBinding.descriptorCount = static_cast<uint32_t>(textureManager.GetTextureList().size());
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::array<VkDescriptorSetLayoutBinding, 3> bindings = { uboLayoutBinding, MaterialLayoutBinding, samplerLayoutBinding };
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(engine.Device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

void Renderer::SetUpDescriptorPool(VulkanEngine& engine) 
{
    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(engine.SwapChain.SwapChainImages.size());
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(engine.SwapChain.SwapChainImages.size());
    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(engine.SwapChain.SwapChainImages.size());

    if (vkCreateDescriptorPool(engine.Device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void Renderer::SetUpDescriptorSets(VulkanEngine& engine, TextureManager& textureManager, std::vector<RayTraceModel>& ModelList)
{
    std::vector<VkDescriptorSetLayout> layouts(engine.SwapChain.SwapChainImages.size(), descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(engine.SwapChain.SwapChainImages.size());
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(engine.SwapChain.SwapChainImages.size());
    if (vkAllocateDescriptorSets(engine.Device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    std::vector<Material> MaterialList;
    for (int x = 0; x < ModelList.size(); x++)
    {
        for (int y = 0; y < ModelList[x].MeshList.size(); y++)
        {
            MaterialList.emplace_back(ModelList[x].MeshList[y].material);
        }
    }
    MaterialBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, sizeof(Material) * MaterialList.size(), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, MaterialList.data());


    for (size_t i = 0; i < engine.SwapChain.SwapChainImages.size(); i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = SceneData->SceneDataBuffer.Buffer;
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(SceneDataBufferData);

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = textureManager.GetTexture(0)->GetTextureView();
        imageInfo.sampler = textureManager.GetTexture(0)->GetTextureSampler();

        std::array<VkWriteDescriptorSet, 3> descriptorWrites{};

        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = descriptorSets[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        VkDescriptorBufferInfo MaterialBufferInfo = {};
        MaterialBufferInfo.buffer = MaterialBuffer.Buffer;
        MaterialBufferInfo.offset = 0;
        MaterialBufferInfo.range = VK_WHOLE_SIZE;

        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = descriptorSets[i];
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorWrites[1].dstBinding = 5;
        descriptorWrites[1].pBufferInfo = &MaterialBufferInfo;
        descriptorWrites[1].descriptorCount = 1;

        std::vector<VkDescriptorImageInfo> DiffuseMapInfoList;
        for (auto texture : textureManager.GetTextureList())
        {
            VkDescriptorImageInfo DiffuseMapImage = {};
            DiffuseMapImage.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            DiffuseMapImage.imageView = texture->GetTextureView();
            DiffuseMapImage.sampler = texture->GetTextureSampler();
            DiffuseMapInfoList.emplace_back(DiffuseMapImage);
        }
        descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[2].dstSet = descriptorSets[i];
        descriptorWrites[2].dstBinding = 6;
        descriptorWrites[2].dstArrayElement = 0;
        descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[2].descriptorCount = static_cast<uint32_t>(textureManager.GetTextureList().size());
        descriptorWrites[2].pImageInfo = DiffuseMapInfoList.data();

        vkUpdateDescriptorSets(engine.Device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
}

void Renderer::SetUpCommandBuffers(VulkanEngine& engine, std::vector<RayTraceModel>& ModelList)
{
    commandBuffers.resize(RenderPass.SwapChainFramebuffers.size());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = engine.CommandPool;
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

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = RenderPass.RenderPass;
        renderPassInfo.framebuffer = RenderPass.SwapChainFramebuffers[i];
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = engine.SwapChain.SwapChainResolution;

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
        clearValues[1].depthStencil = { 1.0f, 0 };

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, RenderPass.forwardRendereringPipeline->ShaderPipeline);
        vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, RenderPass.forwardRendereringPipeline->ShaderPipelineLayout, 0, 1, &descriptorSets[i], 0, nullptr);

        for (auto model : ModelList)
        {
            model.Draw(commandBuffers[i], RenderPass.forwardRendereringPipeline);
        }

        vkCmdEndRenderPass(commandBuffers[i]);
        //    frameBufferRenderPass.Draw(engine, commandBuffers[i], i);
        if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }
}


void Renderer::UpdateSwapChain(VulkanEngine& engine, VulkanWindow& window, TextureManager& textureManager, std::vector<RayTraceModel>& ModelList)
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
    vkDestroyDescriptorPool(engine.Device, descriptorPool, nullptr);

    SetUpDescriptorPool(engine);
    SetUpDescriptorSets(engine, textureManager, ModelList);

    engine.SwapChain.UpdateSwapChain(window.GetWindowPtr(), engine.Device, engine.PhysicalDevice, engine.Surface);
    RenderPass.UpdateSwapChain(engine, descriptorSetLayout);
    interfaceRenderPass.UpdateSwapChain(engine.Device, engine.SwapChain.SwapChainImageViews, engine.SwapChain.SwapChainResolution);
    RayRenderer.Resize(engine, engine.SwapChain.SwapChainImages.size(), engine.SwapChain.SwapChainImages, engine.SwapChain.SwapChainResolution.width, engine.SwapChain.SwapChainResolution.height);

    SetUpCommandBuffers(engine, ModelList);
}

void Renderer::Update(VulkanEngine& engine, VulkanWindow& window, uint32_t currentImage)
{
    keyboard.Update(window.GetWindowPtr(), camera);
    mouse.Update(window.GetWindowPtr(), camera);
    camera->Update(engine.SwapChain.SwapChainResolution.width, engine.SwapChain.SwapChainResolution.height);

    static auto startTime = std::chrono::high_resolution_clock::now();

    auto  currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    SceneData->SceneData.model = glm::mat4(1.0f);
    SceneData->SceneData.viewInverse = glm::inverse(camera->GetViewMatrix());
    SceneData->SceneData.projInverse = glm::inverse(camera->GetProjectionMatrix());
    SceneData->SceneData.projInverse[1][1] *= -1;
    SceneData->SceneData.view = camera->GetViewMatrix();
    SceneData->SceneData.proj = camera->GetProjectionMatrix();
    SceneData->SceneData.proj[1][1] *= -1;
    SceneData->SceneData.viewPos = glm::vec4(camera->GetPosition(), 0.0f);
    SceneData->SceneData.vertexSize = sizeof(Vertex);
    SceneData->Update(engine);
}

void Renderer::GUIUpdate(VulkanEngine& engine)
{
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Checkbox("RayTraceSwitch", &RayTraceSwitch);
    ImGui::SliderFloat3("Pos", &SceneData->SceneData.dlight.direction.x, -1000.0f, 1000.0f);
    ImGui::SliderFloat3("Ambient", &SceneData->SceneData.dlight.ambient.x, 0.0f, 1.0f);
    ImGui::SliderFloat3("Diffuse", &SceneData->SceneData.dlight.diffuse.x, 0.0f, 1.0f);
    ImGui::SliderFloat3("Speculare", &SceneData->SceneData.dlight.specular.x, 0.0f, 1.0f);

    ImGui::SliderFloat3("Pos2", &SceneData->SceneData.plight.position.x, -10.0f, 10.0f);
    ImGui::SliderFloat3("Ambient2", &SceneData->SceneData.plight.ambient.x, 0.0f, 1.0f);
    ImGui::SliderFloat3("Diffuse2", &SceneData->SceneData.plight.diffuse.x, 0.0f, 1.0f);
    ImGui::SliderFloat3("Speculare2", &SceneData->SceneData.plight.specular.x, 0.0f, 1.0f);
    ImGui::SliderFloat("constant", &SceneData->SceneData.plight.constant, 0.0f, 100.0f);
    ImGui::SliderFloat("linear", &SceneData->SceneData.plight.linear, 0.0f, 100.0f);
    ImGui::SliderFloat("quadratic", &SceneData->SceneData.plight.quadratic, 0.0f, 100.0f);
}

void Renderer::Draw(VulkanEngine& engine, VulkanWindow& window, std::vector<RayTraceModel>& ModelList)
{
    vkWaitForFences(engine.Device, 1, &engine.inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(engine.Device, engine.SwapChain.GetSwapChain(), UINT64_MAX, engine.vulkanSemaphores[currentFrame].ImageAcquiredSemaphore, VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        UpdateSwapChain(engine, window, textureManager, ModelList);
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    interfaceRenderPass.Draw(engine.Device, imageIndex, engine.SwapChain.SwapChainResolution);
    RayRenderer.updateUniformBuffers(engine, window.GetWindowPtr(), SceneData->SceneData, camera);
    Update(engine, window, imageIndex);

    if (engine.imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(engine.Device, 1, &engine.imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
    }
    engine.imagesInFlight[imageIndex] = engine.inFlightFences[currentFrame];

    VkSemaphore waitSemaphores[] = { engine.vulkanSemaphores[currentFrame].ImageAcquiredSemaphore };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    std::vector<VkCommandBuffer> CommandBufferSubmitList;
    if (!RayTraceSwitch)
    {
        CommandBufferSubmitList.emplace_back(commandBuffers[imageIndex]);
        CommandBufferSubmitList.emplace_back(interfaceRenderPass.ImGuiCommandBuffers[imageIndex]);
    }
    else
    {
        CommandBufferSubmitList.emplace_back(RayRenderer.drawCmdBuffers[imageIndex]);
        CommandBufferSubmitList.emplace_back(interfaceRenderPass.ImGuiCommandBuffers[imageIndex]);
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
        UpdateSwapChain(engine, window, textureManager, ModelList);
    }
    else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::Destroy(VulkanEngine& engine)
{
    textureManager.Destory(engine);
    interfaceRenderPass.Destroy(engine.Device);
    RenderPass.Destroy(engine);
    RayRenderer.Destory(engine);

    vkDestroyDescriptorPool(engine.Device, descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(engine.Device, descriptorSetLayout, nullptr);
}
