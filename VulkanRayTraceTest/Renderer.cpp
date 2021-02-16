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

    textureManager = TextureManager(engine);
    ModelList.emplace_back(RayTraceModel(engine, textureManager, "C:/Users/dotha/source/repos/VulkanGraphics/Models/Sponza/Sponza.obj"));

    SceneData = std::make_shared<SceneDataStruct>(SceneDataStruct(engine));

    SetUpDescriptorLayout(engine);
    RenderPass = MainRenderPass(engine, descriptorSetLayout);
    //frameBufferRenderPass = FrameBufferRenderPass(engine, textureManager.GetTexture(3));
    interfaceRenderPass = InterfaceRenderPass(engine.Device, engine.Instance, engine.PhysicalDevice, engine.GraphicsQueue, window.GetWindowPtr(), engine.SwapChain.SwapChainImageViews, engine.SwapChain.SwapChainResolution);
    
    SetUpRayTracingDescriptorPool(engine);
    RayRenderer = RayTraceRenderer(engine, textureManager, ModelList, RayTracingDescriptorPool);
    SetUpRayTraceDescriptorSetLayout(engine);
    RayRenderer.createRayTracingPipeline(engine);
    RayRenderer.createShaderBindingTable(engine);
    RayRenderer.createSceneDataBuffer(engine);
    RayRenderer.createDescriptorSets(engine);
    RayRenderer.buildCommandBuffers(engine, engine.SwapChain.SwapChainImages.size(), engine.SwapChain.SwapChainImages);

    std::vector<Material> MaterialList;
    for (int x = 0; x < ModelList.size(); x++)
    {
        for (int y = 0; y < ModelList[x].MeshList.size(); y++)
        {
            MaterialList.emplace_back(ModelList[x].MeshList[y].material);
        }
    }
    MaterialBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, sizeof(Material) * MaterialList.size(), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, MaterialList.data());

    SetUpDescriptorPool(engine);
    SetUpDescriptorSets(engine);
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

void Renderer::SetUpRayTracingDescriptorPool(VulkanEngine& engine)
{
    std::vector<VkDescriptorPoolSize> poolSizes = {
       {VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, 1},
       {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1},
       {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 4},
     { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2 } };

    VkDescriptorPoolCreateInfo RayTraceDescriptorPoolInfo = {};
    RayTraceDescriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    RayTraceDescriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    RayTraceDescriptorPoolInfo.pPoolSizes = poolSizes.data();
    RayTraceDescriptorPoolInfo.maxSets = 1;
    vkCreateDescriptorPool(engine.Device, &RayTraceDescriptorPoolInfo, nullptr, &RayTracingDescriptorPool);
}

void Renderer::SetUpRayTraceDescriptorSetLayout(VulkanEngine& engine)
{
    //std::vector<VkDescriptorSetLayoutBinding> RTDescriptorSetBindings;
    //VkDescriptorSetLayoutBinding              AccelerationStructureBinding = {};
    //AccelerationStructureBinding.binding = 0;
    //AccelerationStructureBinding.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
    //AccelerationStructureBinding.descriptorCount = 1;
    //AccelerationStructureBinding.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
    //RTDescriptorSetBindings.emplace_back(AccelerationStructureBinding);

    //VkDescriptorSetLayoutBinding ReturnImageStructureBinding = {};
    //ReturnImageStructureBinding.binding = 1;
    //ReturnImageStructureBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    //ReturnImageStructureBinding.descriptorCount = 1;
    //ReturnImageStructureBinding.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
    //RTDescriptorSetBindings.emplace_back(ReturnImageStructureBinding);

    //VkDescriptorSetLayoutBinding UniformBufferStructureBinding = {};
    //UniformBufferStructureBinding.binding = 2;
    //UniformBufferStructureBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    //UniformBufferStructureBinding.descriptorCount = 1;
    //UniformBufferStructureBinding.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
    //RTDescriptorSetBindings.emplace_back(UniformBufferStructureBinding);

    //VkDescriptorSetLayoutBinding VertexBufferStructureBinding = {};
    //VertexBufferStructureBinding.binding = 3;
    //VertexBufferStructureBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    //VertexBufferStructureBinding.descriptorCount = ModelList[0].MeshList.size();
    //VertexBufferStructureBinding.stageFlags = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
    //RTDescriptorSetBindings.emplace_back(VertexBufferStructureBinding);

    //VkDescriptorSetLayoutBinding IndexBufferStructureBinding = {};
    //IndexBufferStructureBinding.binding = 4;
    //IndexBufferStructureBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    //IndexBufferStructureBinding.descriptorCount = ModelList[0].MeshList.size();
    //IndexBufferStructureBinding.stageFlags = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
    //RTDescriptorSetBindings.emplace_back(IndexBufferStructureBinding);

    //VkDescriptorSetLayoutBinding MaterialBufferStructureBinding = {};
    //MaterialBufferStructureBinding.binding = 5;
    //MaterialBufferStructureBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    //MaterialBufferStructureBinding.descriptorCount = 1;
    //MaterialBufferStructureBinding.stageFlags = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
    //RTDescriptorSetBindings.emplace_back(MaterialBufferStructureBinding);

    //VkDescriptorSetLayoutBinding DiffuseBinding = {};
    //DiffuseBinding.binding = 6;
    //DiffuseBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    //DiffuseBinding.descriptorCount = static_cast<uint32_t>(textureManager.GetTextureList().size());
    //DiffuseBinding.stageFlags = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
    //RTDescriptorSetBindings.emplace_back(DiffuseBinding);

    //VkDescriptorSetLayoutBinding ReturnShadowImageStructureBinding = {};
    //ReturnShadowImageStructureBinding.binding = 7;
    //ReturnShadowImageStructureBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    //ReturnShadowImageStructureBinding.descriptorCount = 1;
    //ReturnShadowImageStructureBinding.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
    //RTDescriptorSetBindings.emplace_back(ReturnShadowImageStructureBinding);

    //VkDescriptorSetLayoutBinding CubeMapStructureBinding = {};
    //CubeMapStructureBinding.binding = 10;
    //CubeMapStructureBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    //CubeMapStructureBinding.descriptorCount = 1;
    //CubeMapStructureBinding.stageFlags = VK_SHADER_STAGE_MISS_BIT_KHR;
    //RTDescriptorSetBindings.emplace_back(CubeMapStructureBinding);

    //VkDescriptorSetLayoutCreateInfo RTDescriptorSetLayout = {};
    //RTDescriptorSetLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    //RTDescriptorSetLayout.bindingCount = static_cast<uint32_t>(RTDescriptorSetBindings.size());
    //RTDescriptorSetLayout.pBindings = RTDescriptorSetBindings.data();
    //vkCreateDescriptorSetLayout(engine.Device, &RTDescriptorSetLayout, nullptr, &RayRenderer.RayTraceDescriptorSetLayout);
}

void Renderer::SetUpRayTraceDescriptorSet(VulkanEngine& engine)
{
    //VkDescriptorSetAllocateInfo RayTraceDescriptorSetAllocateInfo = {};
    //RayTraceDescriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    //RayTraceDescriptorSetAllocateInfo.descriptorPool = descriptorPool;
    //RayTraceDescriptorSetAllocateInfo.pSetLayouts = &RayTraceDescriptorSetLayout;
    //RayTraceDescriptorSetAllocateInfo.descriptorSetCount = 1;
    //vkAllocateDescriptorSets(engine.Device, &RayTraceDescriptorSetAllocateInfo, &RTDescriptorSet);

    //VkWriteDescriptorSetAccelerationStructureKHR AccelerationDescriptorStructure = {};
    //AccelerationDescriptorStructure.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
    //AccelerationDescriptorStructure.accelerationStructureCount = 1;
    //AccelerationDescriptorStructure.pAccelerationStructures = &topLevelAS.handle;

    //VkWriteDescriptorSet AccelerationDesciptorSet = {};
    //AccelerationDesciptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    //AccelerationDesciptorSet.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
    //AccelerationDesciptorSet.dstSet = RTDescriptorSet;
    //AccelerationDesciptorSet.dstBinding = 0;
    //AccelerationDesciptorSet.descriptorCount = 1;
    //AccelerationDesciptorSet.pNext = &AccelerationDescriptorStructure;

    //VkDescriptorImageInfo RayTraceImageDescriptor{};
    //RayTraceImageDescriptor.imageView = storageImage.View;
    //RayTraceImageDescriptor.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

    //VkWriteDescriptorSet ImageDescriptorSet{};
    //ImageDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    //ImageDescriptorSet.dstSet = RTDescriptorSet;
    //ImageDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    //ImageDescriptorSet.dstBinding = 1;
    //ImageDescriptorSet.pImageInfo = &RayTraceImageDescriptor;
    //ImageDescriptorSet.descriptorCount = 1;

    //VkDescriptorBufferInfo UniformBuffer = {};
    //UniformBuffer.buffer = SceneDataBuffer.Buffer;
    //UniformBuffer.offset = 0;
    //UniformBuffer.range = SceneDataBuffer.BufferSize;

    //VkWriteDescriptorSet UniformDescriptorSet{};
    //UniformDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    //UniformDescriptorSet.dstSet = RTDescriptorSet;
    //UniformDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    //UniformDescriptorSet.dstBinding = 2;
    //UniformDescriptorSet.pBufferInfo = &UniformBuffer;
    //UniformDescriptorSet.descriptorCount = 1;

    //std::vector<VkDescriptorBufferInfo> VertexBufferInfoList;
    //std::vector<VkDescriptorBufferInfo> IndexBufferInfoList;
    //for (int x = 0; x < VertexBufferList.size(); x++)
    //{
    //    VkDescriptorBufferInfo VertexBufferInfo = {};
    //    VertexBufferInfo.buffer = ModelList[0].MeshList[x].VertexBuffer.Buffer;
    //    VertexBufferInfo.offset = 0;
    //    VertexBufferInfo.range = VK_WHOLE_SIZE;
    //    VertexBufferInfoList.emplace_back(VertexBufferInfo);
    //}
    //for (int x = 0; x < IndexBufferList.size(); x++)
    //{
    //    VkDescriptorBufferInfo IndexBufferInfo = {};
    //    IndexBufferInfo.buffer = ModelList[0].MeshList[x].IndexBuffer.Buffer;
    //    IndexBufferInfo.offset = 0;
    //    IndexBufferInfo.range = VK_WHOLE_SIZE;
    //    IndexBufferInfoList.emplace_back(IndexBufferInfo);
    //}


    //VkWriteDescriptorSet VertexDescriptorSet{};
    //VertexDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    //VertexDescriptorSet.dstSet = RTDescriptorSet;
    //VertexDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    //VertexDescriptorSet.dstBinding = 3;
    //VertexDescriptorSet.descriptorCount = static_cast<uint32_t>(VertexBufferInfoList.size());
    //VertexDescriptorSet.pBufferInfo = VertexBufferInfoList.data();

    //VkWriteDescriptorSet IndexDescriptorSet{};
    //IndexDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    //IndexDescriptorSet.dstSet = RTDescriptorSet;
    //IndexDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    //IndexDescriptorSet.dstBinding = 4;
    //IndexDescriptorSet.descriptorCount = static_cast<uint32_t>(IndexBufferInfoList.size());
    //IndexDescriptorSet.pBufferInfo = IndexBufferInfoList.data();

    //VkDescriptorBufferInfo MaterialBufferInfo = {};
    //MaterialBufferInfo.buffer = MaterialBuffer.Buffer;
    //MaterialBufferInfo.offset = 0;
    //MaterialBufferInfo.range = VK_WHOLE_SIZE;

    //VkWriteDescriptorSet MaterialDescriptorSet{};
    //MaterialDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    //MaterialDescriptorSet.dstSet = RTDescriptorSet;
    //MaterialDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    //MaterialDescriptorSet.dstBinding = 5;
    //MaterialDescriptorSet.pBufferInfo = &MaterialBufferInfo;
    //MaterialDescriptorSet.descriptorCount = 1;

    //std::vector<VkDescriptorImageInfo> DiffuseMapInfoList;
    //for (auto texture : textureManager.GetTextureList())
    //{
    //    VkDescriptorImageInfo DiffuseMapImage = {};
    //    DiffuseMapImage.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    //    DiffuseMapImage.imageView = texture->GetTextureView();
    //    DiffuseMapImage.sampler = texture->GetTextureSampler();
    //    DiffuseMapInfoList.emplace_back(DiffuseMapImage);
    //}
    //VkWriteDescriptorSet DiffuseMapDescriptor = {};
    //DiffuseMapDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    //DiffuseMapDescriptor.dstSet = RTDescriptorSet;
    //DiffuseMapDescriptor.dstBinding = 6;
    //DiffuseMapDescriptor.dstArrayElement = 0;
    //DiffuseMapDescriptor.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    //DiffuseMapDescriptor.descriptorCount = static_cast<uint32_t>(textureManager.GetTextureList().size());
    //DiffuseMapDescriptor.pImageInfo = DiffuseMapInfoList.data();

    //VkDescriptorImageInfo ShadowRayTraceImageDescriptor{};
    //ShadowRayTraceImageDescriptor.imageView = shadowStorageImage.View;
    //ShadowRayTraceImageDescriptor.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

    //VkWriteDescriptorSet ImageDescriptorSet2{};
    //ImageDescriptorSet2.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    //ImageDescriptorSet2.dstSet = RTDescriptorSet;
    //ImageDescriptorSet2.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    //ImageDescriptorSet2.dstBinding = 7;
    //ImageDescriptorSet2.pImageInfo = &ShadowRayTraceImageDescriptor;
    //ImageDescriptorSet2.descriptorCount = 1;

    //VkDescriptorImageInfo CubeMapImage = {};
    //CubeMapImage.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    //CubeMapImage.imageView = textureManager.GetCubeMapTexture().GetTextureView();
    //CubeMapImage.sampler = textureManager.GetCubeMapTexture().GetTextureSampler();

    //VkWriteDescriptorSet CubeMapDescriptor = {};
    //CubeMapDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    //CubeMapDescriptor.dstSet = RTDescriptorSet;
    //CubeMapDescriptor.dstBinding = 10;
    //CubeMapDescriptor.dstArrayElement = 0;
    //CubeMapDescriptor.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    //CubeMapDescriptor.descriptorCount = 1;
    //CubeMapDescriptor.pImageInfo = &CubeMapImage;

    //std::vector<VkWriteDescriptorSet> writeDescriptorSets = {
    //    AccelerationDesciptorSet,
    //    ImageDescriptorSet,
    //    UniformDescriptorSet,
    //    VertexDescriptorSet,
    //    IndexDescriptorSet,
    //    MaterialDescriptorSet,
    //    DiffuseMapDescriptor,
    //    ImageDescriptorSet2,
    //    CubeMapDescriptor
    //};
    //vkUpdateDescriptorSets(engine.Device, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, VK_NULL_HANDLE);

}

void Renderer::SetUpDescriptorPool(VulkanEngine& engine)
{
    std::vector<VkDescriptorPoolSize>  DescriptorPoolList = {};

    DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER));
    DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER));

    descriptorPool = engine.CreateDescriptorPool(DescriptorPoolList);
}

void Renderer::SetUpDescriptorLayout(VulkanEngine& engine)
{
    std::vector<DescriptorSetLayoutBindingInfo> LayoutBindingInfo = {};
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 1 });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 5, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, 1 });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 6, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, (uint32_t)textureManager.GetTextureList().size()});
    descriptorSetLayout = engine.CreateDescriptorSetLayout(LayoutBindingInfo);
}

void Renderer::SetUpDescriptorSets(VulkanEngine& engine)
{
    descriptorSets = engine.CreateDescriptorSets(descriptorPool, descriptorSetLayout);

    VkDescriptorBufferInfo SceneDataBufferInfo = AddBufferDescriptorInfo(engine, SceneData->SceneDataBuffer.Buffer, SceneData->SceneDataBuffer.BufferSize);
    VkDescriptorBufferInfo MaterialBufferInfo = AddBufferDescriptorInfo(engine, MaterialBuffer.Buffer, VK_WHOLE_SIZE);
    std::vector<VkDescriptorImageInfo> TextureBufferInfo = AddImageDescriptorInfo(engine, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    for (size_t i = 0; i < engine.SwapChain.GetSwapChainImageCount(); i++)
    {
        std::vector<VkWriteDescriptorSet> DescriptorList;
        DescriptorList.emplace_back(AddDescriptorSetBufferInfo(engine, 0, descriptorSets[i], SceneDataBufferInfo));
        DescriptorList.emplace_back(AddStorageBuffer(engine, 5, descriptorSets[i], MaterialBufferInfo));
        DescriptorList.emplace_back(AddDescriptorSetTextureInfo(engine, 6, descriptorSets[i], TextureBufferInfo));
        
        vkUpdateDescriptorSets(engine.Device, static_cast<uint32_t>(DescriptorList.size()), DescriptorList.data(), 0, nullptr);
    }
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


void Renderer::AddModel(VulkanEngine& engine, VulkanWindow& window, const std::string& FilePath)
{
    ModelList.emplace_back(RayTraceModel(engine, textureManager, "C:/Users/dotha/source/repos/VulkanGraphics/Models/Sponza/Sponza.obj"));
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

    SetUpDescriptorPool(engine);
    SetUpDescriptorSets(engine);

    engine.SwapChain.UpdateSwapChain(window.GetWindowPtr(), engine.Device, engine.PhysicalDevice, engine.Surface);
    RenderPass.UpdateSwapChain(engine, descriptorSetLayout);
    interfaceRenderPass.UpdateSwapChain(engine.Device, engine.SwapChain.SwapChainImageViews, engine.SwapChain.SwapChainResolution);
    RayRenderer.Resize(engine, engine.SwapChain.SwapChainImages.size(), engine.SwapChain.SwapChainImages, engine.SwapChain.SwapChainResolution.width, engine.SwapChain.SwapChainResolution.height);

    SetUpCommandBuffers(engine);
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
        UpdateSwapChain(engine, window);
    }
    else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::Destroy(VulkanEngine& engine)
{
    for (auto model : ModelList)
    {
        model.Destory(engine.Device);
    }

    textureManager.Destory(engine);
    interfaceRenderPass.Destroy(engine.Device);
    RenderPass.Destroy(engine);
    RayRenderer.Destory(engine);

    vkDestroyDescriptorPool(engine.Device, descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(engine.Device, descriptorSetLayout, nullptr);
}

VkDescriptorPoolSize Renderer::AddDsecriptorPoolBinding(VulkanEngine& engine, VkDescriptorType descriptorType)
{
    VkDescriptorPoolSize DescriptorPoolBinding = {};
    DescriptorPoolBinding.type = descriptorType;
    DescriptorPoolBinding.descriptorCount = static_cast<uint32_t>(engine.SwapChain.GetSwapChainImageCount());

    return DescriptorPoolBinding;
}

std::vector<VkDescriptorImageInfo> Renderer::AddImageDescriptorInfo(VulkanEngine& engine, VkImageLayout ImageLayout)
 {
    std::vector<VkDescriptorImageInfo> DescriptorImageList;
    for (auto texture : textureManager.GetTextureList())
    {
        VkDescriptorImageInfo DescriptorImage{};
        DescriptorImage.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        DescriptorImage.imageView = texture->GetTextureView();
        DescriptorImage.sampler = texture->GetTextureSampler();
        DescriptorImageList.emplace_back(DescriptorImage);
    }

    return DescriptorImageList;
}

VkDescriptorBufferInfo Renderer::AddBufferDescriptorInfo(VulkanEngine& engine, VkBuffer Buffer, VkDeviceSize BufferSize)
{
    VkDescriptorBufferInfo BufferInfo = {};
    BufferInfo.buffer = Buffer;
    BufferInfo.offset = 0;
    BufferInfo.range = BufferSize;
    return BufferInfo;
}

VkWriteDescriptorSet Renderer::AddDescriptorSetBufferInfo(VulkanEngine& engine, unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, VkDescriptorBufferInfo& BufferInfo)
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

VkWriteDescriptorSet Renderer::AddDescriptorSetTextureInfo(VulkanEngine& engine, unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, VkDescriptorImageInfo& TextureImageInfo)
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

VkWriteDescriptorSet Renderer::AddDescriptorSetTextureInfo(VulkanEngine& engine, unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, std::vector<VkDescriptorImageInfo>& TextureImageInfo)
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