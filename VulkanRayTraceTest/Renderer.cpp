#include "Renderer.h"
Renderer::Renderer()
{
}

Renderer::Renderer(VulkanEngine& engine, VulkanWindow& window)
{
    //std::vector<Vertex> vertices =
    //{
    //    {{-0.5,-0.5,-0.5}, {0,-1,0}, {0,1}}
    //};

    //std::vector<uint32_t> indices = {
    //   0,1
    //};
    auto a = sizeof(Mesh);
    auto b = sizeof(Model);
    modelRenderManager = ModelRenderManager(engine);
    modelRenderManager.AddModel(engine, "../Models/TestAnimModel/model.dae");
    //modelRenderManager.AddModel(engine, "../Models/vulkanscene_shadow.obj");
   // modelRenderManager.AddModel(engine, "../Models/Sponza/Sponza.obj");

    std::string CubeMapFiles[6];
    CubeMapFiles[0] = "../texture/skybox/right.jpg";
    CubeMapFiles[1] = "../texture/skybox/left.jpg";
    CubeMapFiles[2] = "../texture/skybox/top.jpg";
    CubeMapFiles[3] = "../texture/skybox/bottom.jpg";
    CubeMapFiles[4] = "../texture/skybox/back.jpg";
    CubeMapFiles[5] = "../texture/skybox/front.jpg";

    modelRenderManager.textureManager.LoadCubeMap(engine, CubeMapFiles);

    SceneData = std::make_shared<SceneDataStruct>(SceneDataStruct(engine));

    RenderPass = MainRenderPass(engine);
    //frameBufferRenderPass = FrameBufferRenderPass(engine, textureManager.GetTexture(3));
    interfaceRenderPass = InterfaceRenderPass(engine.Device, engine.Instance, engine.PhysicalDevice, engine.GraphicsQueue, window.GetWindowPtr(), engine.SwapChain.SwapChainImageViews, engine.SwapChain.SwapChainResolution);
    
    SetUpDescriptorPool(engine);
    RayRenderer = RayTraceRenderer(engine, modelRenderManager.ModelList);
    SetUpDescriptorLayout(engine);
    RayRenderer.createRayTracingPipeline(engine, descriptorSetLayout);
    RenderPass.StartPipeline(engine, descriptorSetLayout);
    RayRenderer.createShaderBindingTable(engine);
    SetUpDescriptorSets(engine);
    RayRenderer.buildCommandBuffers(engine, engine.SwapChain.SwapChainImages.size(), engine.SwapChain.SwapChainImages, descriptorSets);

    SetUpCommandBuffers(engine);

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

void Renderer::SetUpDescriptorPool(VulkanEngine& engine)
{
    std::vector<VkDescriptorPoolSize>  DescriptorPoolList = {};
    DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR));
    DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE));
    DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER));
    DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
    DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
    DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
    DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
    DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER));
    DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE));
    DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER));
    descriptorPool = engine.CreateDescriptorPool(DescriptorPoolList);
}

void Renderer::SetUpDescriptorLayout(VulkanEngine& engine)
{
    std::vector<DescriptorSetLayoutBindingInfo> LayoutBindingInfo = {};
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 0, VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR, 1 });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_RAYGEN_BIT_KHR, 1 });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR, 1 });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR, modelRenderManager.GetVertexBufferListDescriptorCount() });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 4, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR, modelRenderManager.GetIndexBufferListDescriptorCount() });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 5, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR, modelRenderManager.GetTransformBufferListDescriptorCount() });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 6, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR, modelRenderManager.GetMaterialBufferListDescriptorCount() });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 7, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR, modelRenderManager.GetTextureBufferListDescriptorCount() });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 8, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 1 });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 10, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_MISS_BIT_KHR, 1 });
    descriptorSetLayout = engine.CreateDescriptorSetLayout(LayoutBindingInfo);
}

void Renderer::SetUpDescriptorSets(VulkanEngine& engine)
{
    descriptorSets = engine.CreateDescriptorSets(descriptorPool, descriptorSetLayout);

    VkWriteDescriptorSetAccelerationStructureKHR AccelerationDescriptorStructure = AddAcclerationStructureBinding(engine, RayRenderer.topLevelAS.handle);
    VkDescriptorImageInfo RayTraceImageDescriptor = AddRayTraceReturnImageDescriptor(engine, VK_IMAGE_LAYOUT_GENERAL, RayRenderer.storageImage);
    std::vector<VkDescriptorBufferInfo> VertexBufferInfoList = modelRenderManager.GetVertexBufferListDescriptor();
    std::vector<VkDescriptorBufferInfo> IndexBufferInfoList = modelRenderManager.GetIndexBufferListDescriptor();
    std::vector<VkDescriptorBufferInfo> MaterialBufferList = modelRenderManager.GetMaterialBufferListDescriptor();
    std::vector<VkDescriptorBufferInfo> TransformBufferList = modelRenderManager.GetTransformBufferListDescriptor();
    VkDescriptorBufferInfo SceneDataBufferInfo = AddBufferDescriptor(engine, SceneData->SceneDataBuffer.Buffer, SceneData->SceneDataBuffer.BufferSize);
    std::vector<VkDescriptorImageInfo> TextureBufferInfo = modelRenderManager.GetTextureBufferListDescriptor();
    VkDescriptorImageInfo CubeMapImage = AddTextureDescriptor(engine, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, std::make_shared<Texture>(modelRenderManager.textureManager.GetCubeMapTexture()));

    std::vector<VkWriteDescriptorSet> DescriptorList;
    DescriptorList.emplace_back(AddAccelerationBuffer(engine, 0, AccelerationDescriptorStructure));
    DescriptorList.emplace_back(AddStorageImageBuffer(engine, 1, descriptorSets, RayTraceImageDescriptor));
    DescriptorList.emplace_back(AddDescriptorSetBuffer(engine, 2, descriptorSets, SceneDataBufferInfo));
    DescriptorList.emplace_back(AddStorageBuffer(engine, 3, descriptorSets, VertexBufferInfoList));
    DescriptorList.emplace_back(AddStorageBuffer(engine, 4, descriptorSets, IndexBufferInfoList));
    DescriptorList.emplace_back(AddStorageBuffer(engine, 5, descriptorSets, TransformBufferList));
    DescriptorList.emplace_back(AddStorageBuffer(engine, 6, descriptorSets, MaterialBufferList));
    DescriptorList.emplace_back(AddDescriptorSetTexture(engine, 7, descriptorSets, TextureBufferInfo));
    DescriptorList.emplace_back(AddDescriptorSetTexture(engine, 10, descriptorSets, CubeMapImage));

    vkUpdateDescriptorSets(engine.Device, static_cast<uint32_t>(DescriptorList.size()), DescriptorList.data(), 0, nullptr);
}

void Renderer::SetUpCommandBuffers(VulkanEngine& engine)
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
        vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, RenderPass.forwardRendereringPipeline->ShaderPipelineLayout, 0, 1, &descriptorSets, 0, nullptr);

        for (auto model : modelRenderManager.ModelList)
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


void Renderer::AddModel(VulkanEngine& engine, VulkanWindow& window, const std::string& FilePath)
{
    modelRenderManager.ModelList.emplace_back(Model(engine, modelRenderManager.textureManager, FilePath));
    UpdateSwapChain(engine, window);
}

void Renderer::UpdateSwapChain(VulkanEngine& engine, VulkanWindow& window)
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

    engine.SwapChain.UpdateSwapChain(window.GetWindowPtr(), engine.Device, engine.PhysicalDevice, engine.Surface);
    RenderPass.UpdateSwapChain(engine, descriptorSetLayout);
    interfaceRenderPass.UpdateSwapChain(engine.Device, engine.SwapChain.SwapChainImageViews, engine.SwapChain.SwapChainResolution);

    vkDestroyImageView(engine.Device, RayRenderer.storageImage.view, nullptr);
    vkDestroyImage(engine.Device, RayRenderer.storageImage.image, nullptr);
    vkFreeMemory(engine.Device, RayRenderer.storageImage.memory, nullptr);

    RayRenderer.createStorageImage(engine, RayRenderer.storageImage);
    SetUpDescriptorPool(engine);
    SetUpDescriptorSets(engine);

    RayRenderer.Resize(engine, engine.SwapChain.SwapChainImages.size(), engine.SwapChain.SwapChainImages, engine.SwapChain.SwapChainResolution.width, engine.SwapChain.SwapChainResolution.height, descriptorSets);

    SetUpCommandBuffers(engine);
}

void Renderer::Update(VulkanEngine& engine, VulkanWindow& window, uint32_t currentImage)
{
    if (UpdateRenderer)
    {
        UpdateSwapChain(engine, window);
    }

    keyboard.Update(window.GetWindowPtr(), camera);
    mouse.Update(window.GetWindowPtr(), camera);
    camera->Update(engine.SwapChain.SwapChainResolution.width, engine.SwapChain.SwapChainResolution.height);

    static auto startTime = std::chrono::high_resolution_clock::now();

    auto  currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();



    for (auto& model : modelRenderManager.ModelList)
    {
        model.Update(engine, SceneData);
    }
    RayRenderer.createTopLevelAccelerationStructure(engine, modelRenderManager.ModelList);

    SceneData->SceneData.model = modelRenderManager.ModelList[0].ModelTransform;
    SceneData->SceneData.viewInverse = glm::inverse(camera->GetViewMatrix());
    SceneData->SceneData.projInverse = glm::inverse(camera->GetProjectionMatrix());
    SceneData->SceneData.projInverse[1][1] *= -1;
    SceneData->SceneData.view = camera->GetViewMatrix();
    SceneData->SceneData.proj = camera->GetProjectionMatrix();
    SceneData->SceneData.proj[1][1] *= -1;
    SceneData->SceneData.viewPos = glm::vec4(camera->GetPosition(), 0.0f);
    SceneData->SceneData.vertexSize = sizeof(Vertex);
    SceneData->SceneData.PVM = SceneData->SceneData.proj * SceneData->SceneData.view * SceneData->SceneData.model;
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

    ImGui::SliderFloat3("Transform", &modelRenderManager.ModelList[0].ModelPosition.x, -10.0f, 10.0f);
    ImGui::SliderFloat3("Rotate", &modelRenderManager.ModelList[0].ModelRotation.x, 0.0f, 360.0f);
    ImGui::SliderFloat3("Scale", &modelRenderManager.ModelList[0].ModelScale.x, 0.0f, 1.0f);

    for (int x = 0; x < modelRenderManager.ModelList.size(); x++)
    {
        for (int y = 0; y < modelRenderManager.ModelList[x].MeshList.size(); y++)
        {
            auto a = std::to_string(y);
            ImGui::Checkbox(a.c_str(), &modelRenderManager.ModelList[x].MeshList[y].ShowMesh);

            ImGui::SliderFloat3(("Transform " + std::to_string(y)).c_str(), &modelRenderManager.ModelList[x].MeshList[y].MeshPosition.x, -10.0f, 10.0f);
            ImGui::SliderFloat3(("Rotate " + std::to_string(y)).c_str(), &modelRenderManager.ModelList[x].MeshList[y].MeshRotation.x, 0.0f, 360.0f);
            ImGui::SliderFloat3(("Scale " + std::to_string(y)).c_str(), &modelRenderManager.ModelList[x].MeshList[y].MeshScale.x, 0.0f, 1.0f);
        }
    }

    ImGui::SliderFloat3("Pos2", &SceneData->SceneData.plight.position.x, -10.0f, 10.0f);
    ImGui::SliderFloat3("Ambient2", &SceneData->SceneData.plight.ambient.x, 0.0f, 1.0f);
    ImGui::SliderFloat3("Diffuse2", &SceneData->SceneData.plight.diffuse.x, 0.0f, 1.0f);
    ImGui::SliderFloat3("Speculare2", &SceneData->SceneData.plight.specular.x, 0.0f, 1.0f);
    ImGui::SliderFloat("constant", &SceneData->SceneData.plight.constant, 0.0f, 100.0f);
    ImGui::SliderFloat("linear", &SceneData->SceneData.plight.linear, 0.0f, 100.0f);
    ImGui::SliderFloat("quadratic", &SceneData->SceneData.plight.quadratic, 0.0f, 100.0f);
}

void Renderer::Draw(VulkanEngine& engine, VulkanWindow& window)
{
    vkWaitForFences(engine.Device, 1, &engine.inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(engine.Device, engine.SwapChain.GetSwapChain(), UINT64_MAX, engine.vulkanSemaphores[currentFrame].ImageAcquiredSemaphore, VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        UpdateSwapChain(engine, window);
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    interfaceRenderPass.Draw(engine.Device, imageIndex, engine.SwapChain.SwapChainResolution);
    Update(engine, window, imageIndex);

    if (engine.imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(engine.Device, 1, &engine.imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
    }
    engine.imagesInFlight[imageIndex] = engine.inFlightFences[currentFrame];

    VkSemaphore waitSemaphores[] = { engine.vulkanSemaphores[currentFrame].ImageAcquiredSemaphore };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    std::vector<VkCommandBuffer> CommandBufferSubmitList;
    if (RayTraceSwitch)
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
        UpdateSwapChain(engine, window);
    }
    else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::Destroy(VulkanEngine& engine)
{
    for (auto model : modelRenderManager.ModelList)
    {
        model.Destory(engine);
    }

    modelRenderManager.textureManager.Destory(engine);
    interfaceRenderPass.Destroy(engine.Device);
    RenderPass.Destroy(engine);
    SceneData->Destroy(engine);
    RayRenderer.Destory(engine);

    vkDestroyDescriptorPool(engine.Device, descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(engine.Device, descriptorSetLayout, nullptr);
}

VkWriteDescriptorSet Renderer::AddAccelerationBuffer(VulkanEngine& engine, unsigned int BindingNumber, VkWriteDescriptorSetAccelerationStructureKHR& accelerationStructure)
{
    VkWriteDescriptorSet AccelerationDesciptorSet = {};
    AccelerationDesciptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    AccelerationDesciptorSet.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
    AccelerationDesciptorSet.dstSet = descriptorSets;
    AccelerationDesciptorSet.dstBinding = BindingNumber;
    AccelerationDesciptorSet.descriptorCount = 1;
    AccelerationDesciptorSet.pNext = &accelerationStructure;
    return AccelerationDesciptorSet;
}

VkWriteDescriptorSetAccelerationStructureKHR Renderer::AddAcclerationStructureBinding(VulkanEngine& engine, VkAccelerationStructureKHR& handle)
{
    VkWriteDescriptorSetAccelerationStructureKHR AccelerationDescriptorStructure = {};
    AccelerationDescriptorStructure.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
    AccelerationDescriptorStructure.accelerationStructureCount = 1;
    AccelerationDescriptorStructure.pAccelerationStructures = &handle;
    return AccelerationDescriptorStructure;
}

VkDescriptorPoolSize Renderer::AddDsecriptorPoolBinding(VulkanEngine& engine, VkDescriptorType descriptorType)
{
    VkDescriptorPoolSize DescriptorPoolBinding = {};
    DescriptorPoolBinding.type = descriptorType;
    DescriptorPoolBinding.descriptorCount = static_cast<uint32_t>(engine.SwapChain.GetSwapChainImageCount());
    return DescriptorPoolBinding;
}

std::vector<VkDescriptorImageInfo> Renderer::AddTextureDescriptor(VulkanEngine& engine, VkImageLayout ImageLayout)
{
    std::vector<VkDescriptorImageInfo> DescriptorImageList;
    for (auto texture : modelRenderManager.textureManager.GetTextureList())
    {
        VkDescriptorImageInfo DescriptorImage{};
        DescriptorImage.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        DescriptorImage.imageView = texture->GetTextureView();
        DescriptorImage.sampler = texture->GetTextureSampler();
        DescriptorImageList.emplace_back(DescriptorImage);
    }
    return DescriptorImageList;
}

VkDescriptorImageInfo Renderer::AddTextureDescriptor(VulkanEngine& engine, VkImageLayout ImageLayout, std::shared_ptr<Texture> texture)
{
    VkDescriptorImageInfo DescriptorImage{};
    DescriptorImage.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    DescriptorImage.imageView = texture->GetTextureView();
    DescriptorImage.sampler = texture->GetTextureSampler();
    return DescriptorImage;
}

VkDescriptorImageInfo Renderer::AddRayTraceReturnImageDescriptor(VulkanEngine& engine, VkImageLayout ImageLayout, StorageImage& texture)
{
    VkDescriptorImageInfo RayTraceImageDescriptor{};
    RayTraceImageDescriptor.imageView = texture.view;
    RayTraceImageDescriptor.imageLayout = ImageLayout;
    return RayTraceImageDescriptor;
}

VkDescriptorBufferInfo Renderer::AddBufferDescriptor(VulkanEngine& engine, VkBuffer Buffer, VkDeviceSize BufferSize)
{
    VkDescriptorBufferInfo BufferInfo = {};
    BufferInfo.buffer = Buffer;
    BufferInfo.offset = 0;
    BufferInfo.range = BufferSize;
    return BufferInfo;
}

VkWriteDescriptorSet Renderer::AddDescriptorSetBuffer(VulkanEngine& engine, unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, VkDescriptorBufferInfo& BufferInfo)
{
    VkWriteDescriptorSet BufferDescriptor = {};
    BufferDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    BufferDescriptor.dstSet = DescriptorSet;
    BufferDescriptor.dstBinding = BindingNumber;
    BufferDescriptor.dstArrayElement = 0;
    BufferDescriptor.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    BufferDescriptor.descriptorCount = 1;
    BufferDescriptor.pBufferInfo = &BufferInfo;
    return BufferDescriptor;
}

VkWriteDescriptorSet Renderer::AddStorageBuffer(VulkanEngine& engine, unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, VkDescriptorBufferInfo& BufferInfo)
{
    VkWriteDescriptorSet BufferDescriptor = {};
    BufferDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    BufferDescriptor.dstSet = DescriptorSet;
    BufferDescriptor.dstBinding = BindingNumber;
    BufferDescriptor.dstArrayElement = 0;
    BufferDescriptor.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    BufferDescriptor.descriptorCount = 1;
    BufferDescriptor.pBufferInfo = &BufferInfo;
    return BufferDescriptor;
}

VkWriteDescriptorSet Renderer::AddStorageBuffer(VulkanEngine& engine, unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, std::vector<VkDescriptorBufferInfo>& BufferInfoList)
{
    VkWriteDescriptorSet BufferDescriptor = {};
    BufferDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    BufferDescriptor.dstSet = DescriptorSet;
    BufferDescriptor.dstBinding = BindingNumber;
    BufferDescriptor.dstArrayElement = 0;
    BufferDescriptor.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    BufferDescriptor.descriptorCount = static_cast<uint32_t>(BufferInfoList.size());
    BufferDescriptor.pBufferInfo = BufferInfoList.data();
    return BufferDescriptor;
}

VkDescriptorBufferInfo Renderer::AddStorageDescriptor(VulkanEngine& engine, VulkanBuffer BufferList)
{
    VkDescriptorBufferInfo BufferInfo = {};
    BufferInfo.buffer = BufferList.Buffer;
    BufferInfo.offset = 0;
    BufferInfo.range = VK_WHOLE_SIZE;
    return BufferInfo;
}

std::vector<VkDescriptorBufferInfo> Renderer::AddStorageDescriptor(VulkanEngine& engine, std::vector<VulkanBuffer> BufferList)
{
    std::vector<VkDescriptorBufferInfo> BufferInfoList;
    for (int x = 0; x < BufferInfoList.size(); x++)
    {
        VkDescriptorBufferInfo BufferInfo = {};
        BufferInfo.buffer = BufferInfoList[x].buffer;
        BufferInfo.offset = 0;
        BufferInfo.range = VK_WHOLE_SIZE;
        BufferInfoList.emplace_back(BufferInfo);
    }
    return BufferInfoList;
}

VkWriteDescriptorSet Renderer::AddStorageImageBuffer(VulkanEngine& engine, unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, VkDescriptorImageInfo& TextureImageInfo)
{
    VkWriteDescriptorSet ImageDescriptor{};
    ImageDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    ImageDescriptor.dstSet = DescriptorSet;
    ImageDescriptor.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    ImageDescriptor.dstBinding = BindingNumber;
    ImageDescriptor.pImageInfo = &TextureImageInfo;
    ImageDescriptor.descriptorCount = 1;
    return ImageDescriptor;
}

VkWriteDescriptorSet Renderer::AddDescriptorSetTexture(VulkanEngine& engine, unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, VkDescriptorImageInfo& TextureImageInfo)
{
    VkWriteDescriptorSet TextureDescriptor = {};
    TextureDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    TextureDescriptor.dstSet = DescriptorSet;
    TextureDescriptor.dstBinding = BindingNumber;
    TextureDescriptor.dstArrayElement = 0;
    TextureDescriptor.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    TextureDescriptor.descriptorCount = 1;
    TextureDescriptor.pImageInfo = &TextureImageInfo;
    return TextureDescriptor;
}

VkWriteDescriptorSet Renderer::AddDescriptorSetTexture(VulkanEngine& engine, unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, std::vector<VkDescriptorImageInfo>& TextureImageInfo)
{
    VkWriteDescriptorSet TextureDescriptor = {};
    TextureDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    TextureDescriptor.dstSet = DescriptorSet;
    TextureDescriptor.dstBinding = BindingNumber;
    TextureDescriptor.dstArrayElement = 0;
    TextureDescriptor.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    TextureDescriptor.descriptorCount = static_cast<uint32_t>(TextureImageInfo.size());
    TextureDescriptor.pImageInfo = TextureImageInfo.data();
    return TextureDescriptor;
}