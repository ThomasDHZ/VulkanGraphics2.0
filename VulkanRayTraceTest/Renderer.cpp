#include "Renderer.h"
#include <stb_image.h>
Renderer::Renderer()
{
}

Renderer::Renderer(VulkanEngine& engine, VulkanWindow& window)
{
    modelRenderManager = ModelRenderManager(engine);
    std::vector<Vertex> MegaManVertices =
	{
		{ {0.5f,  0.5f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 1.0f}, { 0.0f }, {0.05f, 1.0f}, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},
		{{ 0.5f, -0.5f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 1.0f}, { 0.0f }, {0.05f, 0.0f}, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},
		{{-0.5f, -0.5f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 1.0f}, { 0.0f }, {0.0f , 0.0f}, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},
        {{-0.5f,  0.5f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 1.0f}, { 0.0f }, {0.0f , 1.0f}, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}}
	};

    std::vector<uint32_t> indices = 
    {
        0, 1, 3,
        1, 2, 3 
    };

    modelRenderManager.AddModel(engine, MegaManVertices, indices);
    modelRenderManager.AddModel(engine, MegaManVertices, indices);
    modelRenderManager.ModelList[1].MeshList[0].MeshID = 1;

    stbi_set_flip_vertically_on_load(true);
    Material material{};
    material.DiffuseMapID = modelRenderManager.textureManager.LoadTexture2D(engine, "../texture/MegaMan_diffuse.png", VK_FORMAT_R8G8B8A8_UNORM);
    material.NormalMapID = modelRenderManager.textureManager.LoadTexture2D(engine, "../texture/MegaMan_normal.png", VK_FORMAT_R8G8B8A8_UNORM);
    material.DepthMapID = modelRenderManager.textureManager.LoadTexture2D(engine, "../texture/MegaMan_Specular.png", VK_FORMAT_R8G8B8A8_UNORM);
    material.AlphaMapID = modelRenderManager.textureManager.LoadTexture2D(engine, "../texture/MegaMan_Alpha.png", VK_FORMAT_R8G8B8A8_UNORM);
    modelRenderManager.ModelList[0].MeshList[0].material = material;

    modelRenderManager.textureManager.Load3DTexture(engine, "C:/Users/dotha/Desktop/detailed_surfaces/media/sculptureSphere.dds", VK_FORMAT_R8_UNORM);
     /*   modelRenderManager.ModelList[0].MeshList[0].MaterialBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, sizeof(Material), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &material);*/


  //  modelRenderManager.ModelList[0].MeshList[0].ShowMesh = false;
    // modelRenderManager.AddModel(engine, "../Models/cyborg/cyborg.obj");
   //  modelRenderManager.AddModel(engine, "../Models/TestAnimModel/model.dae");
  //    modelRenderManager.ModelList[0].ModelScale = glm::vec3(0.1f);
  // modelRenderManager.AddModel(engine, "../Models/vulkanscene_shadow.obj");
   // modelRenderManager.AddModel(engine, "../Models/Sponza/Sponza.obj");

    std::string CubeMapFiles[6];
    CubeMapFiles[0] = "../texture/skybox/right.jpg";
    CubeMapFiles[1] = "../texture/skybox/left.jpg";
    CubeMapFiles[2] = "../texture/skybox/top.jpg";
    CubeMapFiles[3] = "../texture/skybox/bottom.jpg";
    CubeMapFiles[4] = "../texture/skybox/back.jpg";
    CubeMapFiles[5] = "../texture/skybox/front.jpg";

    modelRenderManager.textureManager.LoadCubeMap(engine, CubeMapFiles);

    SceneData = std::make_shared<SceneDataUniformBuffer>(SceneDataUniformBuffer(engine));

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
    AnimationRenderer = ComputeHelper(engine, modelRenderManager.ModelList[0].MeshList[0].VertexBuffer, SceneData, modelRenderManager.ModelList[0].MeshList[0].TransformBuffer, modelRenderManager.ModelList[0].MeshList[0].MeshProperties);

    SetUpCommandBuffers(engine);

    camera = std::make_shared<PerspectiveCamera>(glm::vec2(engine.SwapChain.SwapChainResolution.width, engine.SwapChain.SwapChainResolution.height), glm::vec3(0.0f, 0.0f, 5.0f));

    SceneData->UniformDataInfo.dlight.direction = glm::vec4(28.572f, 1000.0f, 771.429f, 0.0f);
    SceneData->UniformDataInfo.dlight.ambient = glm::vec4(0.2f);
    SceneData->UniformDataInfo.dlight.diffuse = glm::vec4(0.5f);
    SceneData->UniformDataInfo.dlight.specular = glm::vec4(1.0f);

    SceneData->UniformDataInfo.plight.position = glm::vec4(0.5f, 1.0f, 0.3f, 1.0f);
    SceneData->UniformDataInfo.plight.ambient = glm::vec4(0.2f);
    SceneData->UniformDataInfo.plight.diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 0.0f);
    SceneData->UniformDataInfo.plight.specular = glm::vec4(1.0f);
    SceneData->UniformDataInfo.DepthSampler = 0.0f;
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
    DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
    DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER));
    DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER));
    DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER));
    descriptorPool = engine.CreateDescriptorPool(DescriptorPoolList);
}

void Renderer::SetUpDescriptorLayout(VulkanEngine& engine)
{
    std::vector<DescriptorSetLayoutBindingInfo> LayoutBindingInfo = {};
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 0, VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR, 1 });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_RAYGEN_BIT_KHR, 1 });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR, 1 });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR, 1 });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 4, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR, modelRenderManager.GetVertexBufferListDescriptorCount() });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 5, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR, modelRenderManager.GetIndexBufferListDescriptorCount() });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 6, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR, modelRenderManager.GetTransformBufferListDescriptorCount() });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 7, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR, modelRenderManager.GetMaterialBufferListDescriptorCount() });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 8, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR, modelRenderManager.GetTextureBufferListDescriptorCount() });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 9, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,  VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR, 1 });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 10, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_MISS_BIT_KHR, 1 });
    descriptorSetLayout = engine.CreateDescriptorSetLayout(LayoutBindingInfo);
}

void Renderer::SetUpDescriptorSets(VulkanEngine& engine)
{
    descriptorSets = engine.CreateDescriptorSets(descriptorPool, descriptorSetLayout);

    VkWriteDescriptorSetAccelerationStructureKHR AccelerationDescriptorStructure = engine.AddAcclerationStructureBinding(RayRenderer.topLevelAS.handle);
    VkDescriptorImageInfo RayTraceImageDescriptor = engine.AddRayTraceReturnImageDescriptor(VK_IMAGE_LAYOUT_GENERAL, RayRenderer.storageImage);
    std::vector<VkDescriptorBufferInfo> VertexBufferInfoList = modelRenderManager.GetVertexBufferListDescriptor();
    std::vector<VkDescriptorBufferInfo> IndexBufferInfoList = modelRenderManager.GetIndexBufferListDescriptor();
    std::vector<VkDescriptorBufferInfo> MaterialBufferList = modelRenderManager.GetMaterialBufferListDescriptor();
    std::vector<VkDescriptorBufferInfo> TransformBufferList = modelRenderManager.GetTransformBufferListDescriptor();
    VkDescriptorBufferInfo SceneDataBufferInfo = engine.AddBufferDescriptor(SceneData->VulkanBufferData);
    std::vector<VkDescriptorBufferInfo> MeshPropertyDataBufferInfo = modelRenderManager.GetMeshDataListDescriptor();
    std::vector<VkDescriptorImageInfo> TextureBufferInfo = modelRenderManager.GetTextureBufferListDescriptor();
    std::vector<VkDescriptorImageInfo> Texture3DBufferInfo = modelRenderManager.Get3DTextureBufferListDescriptor();
    VkDescriptorImageInfo CubeMapImage = engine.AddTextureDescriptor(modelRenderManager.textureManager.GetCubeMapTexture().View, modelRenderManager.textureManager.GetCubeMapTexture().Sampler);

    std::vector<VkWriteDescriptorSet> DescriptorList;
    DescriptorList.emplace_back(engine.AddAccelerationBuffer(0, descriptorSets, AccelerationDescriptorStructure));
    DescriptorList.emplace_back(engine.AddStorageImageBuffer(1, descriptorSets, RayTraceImageDescriptor));
    DescriptorList.emplace_back(engine.AddBufferDescriptorSet(2, descriptorSets, SceneDataBufferInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER));
    DescriptorList.emplace_back(engine.AddBufferDescriptorSet(3, descriptorSets, MeshPropertyDataBufferInfo, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
    DescriptorList.emplace_back(engine.AddBufferDescriptorSet(4, descriptorSets, VertexBufferInfoList, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
    DescriptorList.emplace_back(engine.AddBufferDescriptorSet(5, descriptorSets, IndexBufferInfoList, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
    DescriptorList.emplace_back(engine.AddBufferDescriptorSet(6, descriptorSets, TransformBufferList, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
    DescriptorList.emplace_back(engine.AddBufferDescriptorSet(7, descriptorSets, MaterialBufferList, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
    DescriptorList.emplace_back(engine.AddTextureDescriptorSet(8, descriptorSets, TextureBufferInfo));
    DescriptorList.emplace_back(engine.AddTextureDescriptorSet(9, descriptorSets, Texture3DBufferInfo));
    DescriptorList.emplace_back(engine.AddTextureDescriptorSet(10, descriptorSets, CubeMapImage));

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
        AnimationRenderer.Compute(engine, modelRenderManager.ModelList[0].MeshList[0].VertexBuffer, currentFrame);
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
    if(UpdateRenderer)
    {
        UpdateSwapChain(engine, window);
    }

    keyboard.Update(window.GetWindowPtr(), camera);
    mouse.Update(window.GetWindowPtr(), camera);
    camera->Update(engine);

    static auto startTime = std::chrono::high_resolution_clock::now();

    auto  currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    for (auto& model : modelRenderManager.ModelList)
    {
        model.Update(engine, SceneData);
    }
    RayRenderer.createTopLevelAccelerationStructure(engine, modelRenderManager.ModelList);

    SceneData->UniformDataInfo.model = modelRenderManager.ModelList[0].ModelTransform;
    SceneData->UniformDataInfo.viewInverse = glm::inverse(camera->GetViewMatrix());
    SceneData->UniformDataInfo.projInverse = glm::inverse(camera->GetProjectionMatrix());
    SceneData->UniformDataInfo.projInverse[1][1] *= -1;
    SceneData->UniformDataInfo.view = camera->GetViewMatrix();
    SceneData->UniformDataInfo.proj = camera->GetProjectionMatrix();
    SceneData->UniformDataInfo.proj[1][1] *= -1;
    SceneData->UniformDataInfo.viewPos = glm::vec4(camera->GetPosition(), 0.0f);
    SceneData->UniformDataInfo.vertexSize = time;
    SceneData->UniformDataInfo.PVM = SceneData->UniformDataInfo.proj * SceneData->UniformDataInfo.view * SceneData->UniformDataInfo.model;
   // SceneData->SceneData.timer = time;
    SceneData->Update(engine);
}

void Renderer::GUIUpdate(VulkanEngine& engine)
{
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Checkbox("RayTraceSwitch", &RayTraceSwitch);

    ImGui::SliderFloat("DepthSampler", &SceneData->UniformDataInfo.DepthSampler, 0, 1);
    ImGui::SliderFloat3("Pos", &SceneData->UniformDataInfo.dlight.direction.x, -1000.0f, 1000.0f);
    ImGui::SliderFloat3("Ambient", &SceneData->UniformDataInfo.dlight.ambient.x, 0.0f, 1.0f);
    ImGui::SliderFloat3("Diffuse", &SceneData->UniformDataInfo.dlight.diffuse.x, 0.0f, 1.0f);
    ImGui::SliderFloat3("Speculare", &SceneData->UniformDataInfo.dlight.specular.x, 0.0f, 1.0f);


    for (int x = 0; x < modelRenderManager.ModelList.size(); x++)
    {
        ImGui::SliderFloat3(("Model Transformx" + std::to_string(x)).c_str(), &modelRenderManager.ModelList[x].ModelPosition.x, -10.0f, 10.0f);
        ImGui::SliderFloat3(("Model Rotatex" + std::to_string(x)).c_str(), &modelRenderManager.ModelList[x].ModelRotation.x, 0.0f, 360.0f);
        ImGui::SliderFloat3(("Model Scalex" + std::to_string(x)).c_str(), &modelRenderManager.ModelList[x].ModelScale.x, 0.0f, 1.0f);

        for (int y = 0; y < modelRenderManager.ModelList[x].MeshList.size(); y++)
        {
            auto a = std::to_string(x + y);
            ImGui::Checkbox(a.c_str(), &modelRenderManager.ModelList[x].MeshList[y].ShowMesh);

            ImGui::SliderFloat2(("UV Offset " + std::to_string(x + y)).c_str(), &modelRenderManager.ModelList[x].MeshList[y].MeshProperties.UniformDataInfo.UVOffset.x, 0.0f, 1.0f);
            ImGui::SliderFloat3(("Transform " + std::to_string(x + y)).c_str(), &modelRenderManager.ModelList[x].MeshList[y].MeshPosition.x, -10.0f, 10.0f);
            ImGui::SliderFloat3(("Rotate " + std::to_string(x + y)).c_str(), &modelRenderManager.ModelList[x].MeshList[y].MeshRotation.x, 0.0f, 360.0f);
            ImGui::SliderFloat3(("Scale " + std::to_string(x + y)).c_str(), &modelRenderManager.ModelList[x].MeshList[y].MeshScale.x, 0.0f, 1.0f);
        }
    }

    ImGui::SliderFloat3("Pos2", &SceneData->UniformDataInfo.plight.position.x, -10.0f, 10.0f);
    ImGui::SliderFloat3("Ambient2", &SceneData->UniformDataInfo.plight.ambient.x, 0.0f, 1.0f);
    ImGui::SliderFloat3("Diffuse2", &SceneData->UniformDataInfo.plight.diffuse.x, 0.0f, 1.0f);
    ImGui::SliderFloat3("Speculare2", &SceneData->UniformDataInfo.plight.specular.x, 0.0f, 1.0f);
    ImGui::SliderFloat("constant", &SceneData->UniformDataInfo.plight.constant, 0.0f, 100.0f);
    ImGui::SliderFloat("linear", &SceneData->UniformDataInfo.plight.linear, 0.0f, 100.0f);
    ImGui::SliderFloat("quadratic", &SceneData->UniformDataInfo.plight.quadratic, 0.0f, 100.0f);
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

  //  modelRenderManager.ModelList[0].MeshList[0].VertexBuffer.CopyBufferToMemory(engine.Device, &modelRenderManager.ModelList[0].MeshList[0].VertexList[0], sizeof(Vertex) * modelRenderManager.ModelList[0].MeshList[0].VertexList.size());
    if (RayTraceSwitch)
    {
       // CommandBufferSubmitList.emplace_back(AnimationRenderer.commandBuffer);
        CommandBufferSubmitList.emplace_back(commandBuffers[imageIndex]);
        CommandBufferSubmitList.emplace_back(interfaceRenderPass.ImGuiCommandBuffers[imageIndex]);
    }
    else
    {
      //  CommandBufferSubmitList.emplace_back(AnimationRenderer.commandBuffer);
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
    AnimationRenderer.Destroy(engine);
    RenderPass.Destroy(engine);
    SceneData->Destroy(engine);
    RayRenderer.Destory(engine);

    vkDestroyDescriptorPool(engine.Device, descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(engine.Device, descriptorSetLayout, nullptr);
}

std::vector<Vertex> Renderer::CalcVertex()
{
    glm::vec3 pos1(-1.0f, 1.0f, 0.0f);
    glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
    glm::vec3 pos3(1.0f, -1.0f, 0.0f);
    glm::vec3 pos4(1.0f, 1.0f, 0.0f);
    // texture coordinates
    glm::vec2 uv1(0.0f, 1.0f);
    glm::vec2 uv2(0.0f, 0.0f);
    glm::vec2 uv3(1.0f, 0.0f);
    glm::vec2 uv4(1.0f, 1.0f);
    // normal vector
    glm::vec3 nm(0.0f, 0.0f, 1.0f);

    // calculate tangent/bitangent vectors of both triangles
    glm::vec3 tangent1, bitangent1;
    glm::vec3 tangent2, bitangent2;
    // triangle 1
    // ----------
    glm::vec3 edge1 = pos2 - pos1;
    glm::vec3 edge2 = pos3 - pos1;
    glm::vec2 deltaUV1 = uv2 - uv1;
    glm::vec2 deltaUV2 = uv3 - uv1;

    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    tangent1 = glm::normalize(tangent1);

    bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
    bitangent1 = glm::normalize(bitangent1);

    // triangle 2
    // ----------
    edge1 = pos3 - pos1;
    edge2 = pos4 - pos1;
    deltaUV1 = uv3 - uv1;
    deltaUV2 = uv4 - uv1;

    f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    tangent2 = glm::normalize(tangent2);


    bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
    bitangent2 = glm::normalize(bitangent2);

    return {
        // positions            // normal         // texcoords  // tangent                          // bitangent
        {{pos1.x, pos1.y, pos1.z}, {0.0f}, {nm.x, nm.y, nm.z}, {0.0f}, {uv1.x, uv1.y}, {0.0f, 0.0f}, {tangent1.x, tangent1.y, tangent1.z}, {0.0f}, {bitangent1.x, bitangent1.y, bitangent1.z}, {0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
        {{pos2.x, pos2.y, pos2.z}, {0.0f}, {nm.x, nm.y, nm.z}, {0.0f }, {uv2.x, uv2.y}, { 0.0f, 0.0f }, {tangent1.x, tangent1.y, tangent1.z}, {0.0f}, {bitangent1.x, bitangent1.y, bitangent1.z}, {0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
        {{pos3.x, pos3.y, pos3.z}, {0.0f }, {nm.x, nm.y, nm.z}, {0.0f}, {uv3.x, uv3.y}, { 0.0f, 0.0f }, {tangent1.x, tangent1.y, tangent1.z}, {0.0f}, {bitangent1.x, bitangent1.y, bitangent1.z}, {0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},

        {{pos1.x, pos1.y, pos1.z}, {0.0f }, {nm.x, nm.y, nm.z}, {0.0f}, {uv1.x, uv1.y}, { 0.0f, 0.0f }, {tangent2.x, tangent2.y, tangent2.z}, {0.0f}, {bitangent2.x, bitangent2.y, bitangent2.z}, {0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
        {{pos3.x, pos3.y, pos3.z}, {0.0f }, {nm.x, nm.y, nm.z}, {0.0f}, {uv3.x, uv3.y}, { 0.0f, 0.0f }, {tangent2.x, tangent2.y, tangent2.z}, {0.0f}, {bitangent2.x, bitangent2.y, bitangent2.z}, {0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
        {{pos4.x, pos4.y, pos4.z}, {0.0f }, {nm.x, nm.y, nm.z}, {0.0f}, {uv4.x, uv4.y}, { 0.0f, 0.0f }, {tangent2.x, tangent2.y, tangent2.z}, {0.0f}, {bitangent2.x, bitangent2.y, bitangent2.z}, {0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}}
    };
}