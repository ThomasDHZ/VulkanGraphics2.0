#include "Renderer.h"

Renderer::Renderer()
{
}

Renderer::Renderer(VulkanEngine& engine, VulkanWindow& window, TextureManager& textureManager, std::vector<RayTraceModel>& ModelList)
{
    storageImage = std::make_shared<RenderedRayTracedColorTexture>(engine);
    shadowStorageImage = std::make_shared<RenderedRayTracedColorTexture>(engine);
    textureManager.AddTexture(engine, storageImage);
    textureManager.AddTexture(engine, shadowStorageImage);

    SetUpDescriptorLayout(engine, textureManager);
    RenderPass = MainRenderPass(engine, descriptorSetLayout);
    frameBufferRenderPass = FrameBufferRenderPass(engine, textureManager.GetTexture(3));
    RayRenderer = RayTraceRenderer(engine, textureManager, ModelList, storageImage, shadowStorageImage);
    interfaceRenderPass = InterfaceRenderPass(engine.Device, engine.Instance, engine.PhysicalDevice, engine.GraphicsQueue, window.GetWindowPtr(), engine.SwapChain.SwapChainImageViews, engine.SwapChain.SwapChainResolution);
    SetUpDescriptorPool(engine);
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

void Renderer::SetUpDescriptorSets(VulkanEngine& engine, TextureManager& textureManager, std::vector<RayTraceModel>& ModelList, VulkanBuffer& MaterialBuffer, std::shared_ptr<SceneDataStruct>& SceneData)
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

void Renderer::Draw(VulkanEngine& engine)
{
    //vkWaitForFences(engine.Device, 1, &engine.inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    //uint32_t imageIndex;
    //VkResult result = vkAcquireNextImageKHR(engine.Device, engine.SwapChain.GetSwapChain(), UINT64_MAX, engine.vulkanSemaphores[currentFrame].ImageAcquiredSemaphore, VK_NULL_HANDLE, &imageIndex);

    //if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    //    recreateSwapChain();
    //    return;
    //}
    //else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    //    throw std::runtime_error("failed to acquire swap chain image!");
    //}

    //interfaceRenderPass.Draw(engine.Device, imageIndex, engine.SwapChain.SwapChainResolution);
    //RayRenderer.updateUniformBuffers(engine, window.GetWindowPtr(), SceneData->SceneData, camera);
    //updateUniformBuffer(imageIndex);

    //if (engine.imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
    //    vkWaitForFences(engine.Device, 1, &engine.imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
    //}
    //engine.imagesInFlight[imageIndex] = engine.inFlightFences[currentFrame];

    //VkSemaphore waitSemaphores[] = { engine.vulkanSemaphores[currentFrame].ImageAcquiredSemaphore };
    //VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    //std::vector<VkCommandBuffer> CommandBufferSubmitList;
    //if (!RayTraceSwitch)
    //{
    //    CommandBufferSubmitList.emplace_back(commandBuffers[imageIndex]);
    //    CommandBufferSubmitList.emplace_back(interfaceRenderPass.ImGuiCommandBuffers[imageIndex]);
    //}
    //else
    //{
    //    CommandBufferSubmitList.emplace_back(RayRenderer.drawCmdBuffers[imageIndex]);
    //    CommandBufferSubmitList.emplace_back(interfaceRenderPass.ImGuiCommandBuffers[imageIndex]);
    //}

    //VkSubmitInfo submitInfo{};
    //submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    //submitInfo.waitSemaphoreCount = 1;
    //submitInfo.pWaitSemaphores = waitSemaphores;
    //submitInfo.pWaitDstStageMask = waitStages;
    //submitInfo.commandBufferCount = static_cast<uint32_t>(CommandBufferSubmitList.size());
    //submitInfo.pCommandBuffers = CommandBufferSubmitList.data();

    //VkSemaphore signalSemaphores[] = { engine.vulkanSemaphores[currentFrame].RenderCompleteSemaphore };
    //submitInfo.signalSemaphoreCount = 1;
    //submitInfo.pSignalSemaphores = signalSemaphores;


    //vkResetFences(engine.Device, 1, &engine.inFlightFences[currentFrame]);

    //if (vkQueueSubmit(engine.GraphicsQueue, 1, &submitInfo, engine.inFlightFences[currentFrame]) != VK_SUCCESS) {
    //    throw std::runtime_error("failed to submit draw command buffer!");
    //}

    //VkPresentInfoKHR presentInfo{};
    //presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    //presentInfo.waitSemaphoreCount = 1;
    //presentInfo.pWaitSemaphores = signalSemaphores;

    //VkSwapchainKHR swapChains[] = { engine.SwapChain.GetSwapChain() };
    //presentInfo.swapchainCount = 1;
    //presentInfo.pSwapchains = swapChains;

    //presentInfo.pImageIndices = &imageIndex;

    //result = vkQueuePresentKHR(engine.PresentQueue, &presentInfo);

    //if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
    //    framebufferResized = false;
    //    recreateSwapChain();
    //}
    //else if (result != VK_SUCCESS) {
    //    throw std::runtime_error("failed to present swap chain image!");
    //}

    //currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}
