#include "Renderer.h"
#include <stb_image.h>
#include "Sprite.h"
#include "MegaMan.h"
#include "Mario.h"

Renderer::Renderer()
{
}

Renderer::Renderer(VulkanEngine& engine, VulkanWindow& window)
{
    RenderPass = ForwardRenderPass(engine);
    interfaceRenderPass = InterfaceRenderPass(engine, window.GetWindowPtr());

    assetManager = AssetManager(engine);
    SceneData = std::make_shared<SceneDataUniformBuffer>(SceneDataUniformBuffer(engine));

    frameBufferRenderPass = FrameBufferRenderPass(engine, assetManager, SceneData);

    std::shared_ptr<Material> material = std::make_shared<Material>(engine, assetManager.textureManager);
    material->materialTexture.DiffuseMap = assetManager.textureManager.LoadTexture2D(engine, "../texture/toy_box_diffuse.png", VK_FORMAT_R8G8B8A8_UNORM);
    material->materialTexture.NormalMap = assetManager.textureManager.LoadTexture2D(engine, "../texture/toy_box_normal.png", VK_FORMAT_R8G8B8A8_UNORM);
    material->materialTexture.DepthMap = assetManager.textureManager.LoadTexture2D(engine, "../texture/toy_box_disp.png", VK_FORMAT_R8G8B8A8_UNORM);
    uint32_t MaterialID = assetManager.materialManager.LoadMaterial(engine, "MarioMaterial", material);

    assetManager.AddModel(engine, "../Models/Crate.dae");
    assetManager.modelManager.ModelList[0]->MeshList[0]->MaterialID = MaterialID;

    assetManager.meshManager.MeshList.emplace_back(std::make_shared<MegaMan>(MegaMan(engine, assetManager, glm::vec3(1.0f, 0.0f, 0.0f))));
    assetManager.meshManager.MeshList.emplace_back(std::make_shared<MegaMan>(MegaMan(engine, assetManager, glm::vec3(2.0f, 0.0f, 0.0f))));
    assetManager.meshManager.MeshList.emplace_back(std::make_shared<MegaMan>(MegaMan(engine, assetManager, glm::vec3(3.0f, 0.0f, 0.0f))));
    assetManager.meshManager.MeshList.emplace_back(std::make_shared<Mario>(Mario(engine, assetManager, glm::vec3(4.0f, 0.0f, 0.0f))));

    assetManager.AddModel();
    assetManager.modelManager.ModelList.back()->AddMesh(engine, assetManager.meshManager.MeshList[0]);
    assetManager.modelManager.ModelList.back()->AddMesh(engine, assetManager.meshManager.MeshList[1]);
    assetManager.modelManager.ModelList.back()->AddMesh(engine, assetManager.meshManager.MeshList[2]);
    assetManager.modelManager.ModelList.back()->AddMesh(engine, assetManager.meshManager.MeshList[3]);

    assetManager.AddModel(engine, "../Models/vulkanscene_shadow.obj");
    //assetManager.AddModel(engine, "../Models/TestAnimModel/model.dae");
    //assetManager.AddModel(engine, "../Models/cyborg/cyborg.obj");
    // modelRenderManager.AddModel(engine, "../Models/Sponza/Sponza.obj");



    std::string CubeMapFiles[6];
    CubeMapFiles[0] = "../texture/skybox/right.jpg";
    CubeMapFiles[1] = "../texture/skybox/left.jpg";
    CubeMapFiles[2] = "../texture/skybox/top.jpg";
    CubeMapFiles[3] = "../texture/skybox/bottom.jpg";
    CubeMapFiles[4] = "../texture/skybox/back.jpg";
    CubeMapFiles[5] = "../texture/skybox/front.jpg";

    assetManager.textureManager.LoadCubeMap(engine, CubeMapFiles);

    SetUpDescriptorPool(engine);
    RayRenderer = RayTraceRenderer(engine, assetManager);
    SetUpDescriptorLayout(engine);
    RayRenderer.createRayTracingPipeline(engine, descriptorSetLayout);
    RenderPass.StartPipeline(engine, descriptorSetLayout);
    RayRenderer.createShaderBindingTable(engine);
    SetUpDescriptorSets(engine);
    //AnimationRenderer = AnimatorCompute(engine, assetManager.modelManager.ModelList[2]->MeshList[0]);
    SetUpCommandBuffers(engine);

  //  skybox = Skybox(engine, assetManager, RenderPass.RenderPass, SceneData);


    camera = std::make_shared<PerspectiveCamera>(glm::vec2(engine.SwapChain.SwapChainResolution.width, engine.SwapChain.SwapChainResolution.height), glm::vec3(0.0f, 0.0f, 5.0f));

    SceneData->UniformDataInfo.dlight.direction = glm::vec4(0.0f);
    SceneData->UniformDataInfo.dlight.ambient = glm::vec4(0.2f);
    SceneData->UniformDataInfo.dlight.diffuse = glm::vec4(0.5f);
    SceneData->UniformDataInfo.dlight.specular = glm::vec4(1.0f);

    SceneData->UniformDataInfo.plight[0].position = glm::vec4(0.5f, 1.0f, 0.3f, 1.0f);
    SceneData->UniformDataInfo.plight[0].ambient = glm::vec4(0.2f);
    SceneData->UniformDataInfo.plight[0].diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 0.0f);
    SceneData->UniformDataInfo.plight[0].specular = glm::vec4(1.0f);

    SceneData->UniformDataInfo.plight[1].position = glm::vec4(0.5f, 1.0f, 0.3f, 1.0f);
    SceneData->UniformDataInfo.plight[1].ambient = glm::vec4(0.2f);
    SceneData->UniformDataInfo.plight[1].diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 0.0f);
    SceneData->UniformDataInfo.plight[1].specular = glm::vec4(1.0f);

    SceneData->UniformDataInfo.plight[2].position = glm::vec4(0.5f, 1.0f, 0.3f, 1.0f);
    SceneData->UniformDataInfo.plight[2].ambient = glm::vec4(0.2f);
    SceneData->UniformDataInfo.plight[2].diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 0.0f);
    SceneData->UniformDataInfo.plight[2].specular = glm::vec4(1.0f);

    SceneData->UniformDataInfo.plight[3].position = glm::vec4(0.5f, 1.0f, 0.3f, 1.0f);
    SceneData->UniformDataInfo.plight[3].ambient = glm::vec4(0.2f);
    SceneData->UniformDataInfo.plight[3].diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 0.0f);
    SceneData->UniformDataInfo.plight[3].specular = glm::vec4(1.0f);

    SceneData->UniformDataInfo.plight[4].position = glm::vec4(0.5f, 1.0f, 0.3f, 1.0f);
    SceneData->UniformDataInfo.plight[4].ambient = glm::vec4(0.2f);
    SceneData->UniformDataInfo.plight[4].diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 0.0f);
    SceneData->UniformDataInfo.plight[4].specular = glm::vec4(1.0f);

    SceneData->UniformDataInfo.sLight.ambient = glm::vec4(0.0f);
    SceneData->UniformDataInfo.sLight.diffuse = glm::vec4(1.0f);
    SceneData->UniformDataInfo.sLight.specular = glm::vec4(1.0f);

    ImGui_ImplVulkan_AddTexture(assetManager.textureManager.TextureList[4]->ImGuiDescriptorSet, assetManager.textureManager.TextureList[4]->Sampler, assetManager.textureManager.TextureList[4]->View, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

Renderer::~Renderer()
{

}

void Renderer::SetUpCommandBuffers(VulkanEngine& engine)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = engine.CommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(engine.Device, &allocInfo, &RasterCommandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
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
    frameBufferRenderPass.UpdateSwapChain(engine);
    interfaceRenderPass.UpdateSwapChain(engine);

    RayRenderer.createStorageImage(engine);
    SetUpDescriptorPool(engine);
    SetUpDescriptorSets(engine);

    RayRenderer.Resize(engine, assetManager, descriptorSets, 0);

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

    assetManager.Update(engine);
   // skybox.Update(engine, assetManager.materialManager);
    RayRenderer.createTopLevelAccelerationStructure(engine, assetManager);

    SceneData->UniformDataInfo.sLight.direction = camera->GetFront();
    SceneData->UniformDataInfo.viewInverse = glm::inverse(camera->GetViewMatrix());
    SceneData->UniformDataInfo.projInverse = glm::inverse(camera->GetProjectionMatrix());
    SceneData->UniformDataInfo.projInverse[1][1] *= -1;
    SceneData->UniformDataInfo.view = camera->GetViewMatrix();
    SceneData->UniformDataInfo.proj = camera->GetProjectionMatrix();
    SceneData->UniformDataInfo.proj[1][1] *= -1;
    SceneData->UniformDataInfo.viewPos = glm::vec4(camera->GetPosition(), 0.0f);
    SceneData->UniformDataInfo.timer = engine.VulkanTimer();
    SceneData->Update(engine);
}

void Renderer::GUIUpdate(VulkanEngine& engine)
{

    //for (int y = 0; y < assetManager.materialManager.MaterialList.size(); y++)
    //{
    //    auto a = std::to_string(y);;

    //    ImGui::SliderFloat(("Reflection  " + std::to_string(y)).c_str(), &assetManager.materialManager.MaterialList[y]->materialTexture.Reflectivness, 0.0f, 1.0f);
    //}

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::SliderInt("TextureIndex", &SceneData->UniformDataInfo.temp, 0, 23);
    ImGui::Checkbox("RayTraceSwitch", &RayTraceSwitch);
    ImGui::SliderInt("Shadow", &SceneData->UniformDataInfo.Shadowed, 0, 1);
    ImGui::Checkbox("AddMaterial", &AddMaterialFlag);
    ImGui::Checkbox("DeleteMaterial", &RemoveMaterialFlag);

    ImGui::Image(assetManager.textureManager.TextureList[4]->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    //ImGui::Image(assetManager.textureManager.TextureList[3]->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    //ImGui::Image(assetManager.textureManager.TextureList[3]->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    //ImGui::Image(assetManager.textureManager.TextureList[3]->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    //ImGui::Image(assetManager.textureManager.TextureList[3]->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));

    ImGui::SliderFloat3("Position2", &SceneData->UniformDataInfo.sLight.position.x, 0.0f, 10.0f);
    ImGui::SliderFloat3("Direction2", &SceneData->UniformDataInfo.sLight.direction.x, -1.0f, 1.0f);
    ImGui::SliderFloat3("Ambient245", &SceneData->UniformDataInfo.sLight.ambient.x, 0.0f, 1.0f);
    ImGui::SliderFloat3("Diffuse245", &SceneData->UniformDataInfo.sLight.diffuse.x, 0.0f, 1.0f);
    ImGui::SliderFloat3("Speculare245", &SceneData->UniformDataInfo.sLight.specular.x, 0.0f, 1.0f);
    ImGui::SliderFloat("constant45", &SceneData->UniformDataInfo.sLight.constant, 0.0f, 100.0f);
    ImGui::SliderFloat("linear45", &SceneData->UniformDataInfo.sLight.linear, 0.0f, 100.0f);
    ImGui::SliderFloat("quadratic45", &SceneData->UniformDataInfo.sLight.quadratic, 0.0f, 100.0f);
    ImGui::SliderFloat("quadratic452", &SceneData->UniformDataInfo.sLight.cutOff, 0.0f, 100.0f);
    ImGui::SliderFloat("quadratic453", &SceneData->UniformDataInfo.sLight.outerCutOff, 0.0f, 100.0f);

    ImGui::SliderFloat3("Pos", &SceneData->UniformDataInfo.dlight.direction.x, -1.0f, 1.0f);
    ImGui::SliderFloat3("Ambient", &SceneData->UniformDataInfo.dlight.ambient.x, 0.0f, 1.0f);
    ImGui::SliderFloat3("Diffuse", &SceneData->UniformDataInfo.dlight.diffuse.x, 0.0f, 1.0f);
    ImGui::SliderFloat3("Speculare", &SceneData->UniformDataInfo.dlight.specular.x, 0.0f, 1.0f);

    for (int y = 0; y < assetManager.meshManager.MeshList.size(); y++)
    {
        auto a = std::to_string(y);
        ImGui::Checkbox(a.c_str(), &assetManager.meshManager.MeshList[y]->ShowMesh);

        ImGui::SliderFloat(("Depth " + std::to_string(y)).c_str(), &assetManager.meshManager.MeshList[y]->MeshProperties.UniformDataInfo.heightScale, 0.0f, 1.0f);
        ImGui::SliderFloat2(("UV Offset " + std::to_string(y)).c_str(), &assetManager.meshManager.MeshList[y]->MeshProperties.UniformDataInfo.UVOffset.x, 0.0f, 1.0f);
        ImGui::SliderFloat3(("Transform " + std::to_string(y)).c_str(), &assetManager.meshManager.MeshList[y]->MeshPosition.x, -10.0f, 10.0f);
        ImGui::SliderFloat3(("Rotate " + std::to_string(y)).c_str(), &assetManager.meshManager.MeshList[y]->MeshRotation.x, 0.0f, 360.0f);
        ImGui::SliderFloat3(("Scale " + std::to_string(y)).c_str(), &assetManager.meshManager.MeshList[y]->MeshScale.x, 0.0f, 1.0f);
    }

    //for (int x = 0; x < assetManager.modelManager.ModelList.size(); x++)
    //{
    //    ImGui::SliderFloat3(("Model Transformx" + std::to_string(x)).c_str(), &assetManager.modelManager.ModelList[x]->ModelPosition.x, -10.0f, 10.0f);
    //    ImGui::SliderFloat3(("Model Rotatex" + std::to_string(x)).c_str(), &assetManager.modelManager.ModelList[x]->ModelRotation.x, 0.0f, 360.0f);
    //    ImGui::SliderFloat3(("Model Scalex" + std::to_string(x)).c_str(), &assetManager.modelManager.ModelList[x]->ModelScale.x, 0.0f, 1.0f);

    //    for (int y = 0; y < assetManager.modelManager.ModelList[x]->MeshList.size(); y++)
    //    {
    //        auto a = std::to_string(x + y);
    //        ImGui::Checkbox(a.c_str(), &assetManager.modelManager.ModelList[x]->MeshList[y]->ShowMesh);

    //        ImGui::SliderFloat2(("UV Offset " + std::to_string(x + y)).c_str(), &assetManager.modelManager.ModelList[x]->MeshList[y]->MeshProperties.UniformDataInfo.UVOffset.x, 0.0f, 1.0f);
    //        ImGui::SliderFloat3(("Transform " + std::to_string(x + y)).c_str(), &assetManager.modelManager.ModelList[x]->MeshList[y]->MeshPosition.x, -10.0f, 10.0f);
    //        ImGui::SliderFloat3(("Rotate " + std::to_string(x + y)).c_str(), &assetManager.modelManager.ModelList[x]->MeshList[y]->MeshRotation.x, 0.0f, 360.0f);
    //        ImGui::SliderFloat3(("Scale " + std::to_string(x + y)).c_str(), &assetManager.modelManager.ModelList[x]->MeshList[y]->MeshScale.x, 0.0f, 1.0f);
    //    }
    //}

    ImGui::SliderFloat3("Pos20", &SceneData->UniformDataInfo.plight[0].position.x, -10.0f, 10.0f);
    ImGui::SliderFloat3("Ambient20", &SceneData->UniformDataInfo.plight[0].ambient.x, 0.0f, 1.0f);
    ImGui::SliderFloat3("Diffuse20", &SceneData->UniformDataInfo.plight[0].diffuse.x, 0.0f, 1.0f);
    ImGui::SliderFloat3("Speculare20", &SceneData->UniformDataInfo.plight[0].specular.x, 0.0f, 1.0f);
    ImGui::SliderFloat("constant0", &SceneData->UniformDataInfo.plight[0].constant, 0.0f, 100.0f);
    ImGui::SliderFloat("linear0", &SceneData->UniformDataInfo.plight[0].linear, 0.0f, 100.0f);
    ImGui::SliderFloat("quadratic0", &SceneData->UniformDataInfo.plight[0].quadratic, 0.0f, 100.0f);

    ImGui::SliderFloat3("Pos21", &SceneData->UniformDataInfo.plight[1].position.x, -10.0f, 10.0f);
    ImGui::SliderFloat3("Ambient21", &SceneData->UniformDataInfo.plight[1].ambient.x, 0.0f, 1.0f);
    ImGui::SliderFloat3("Diffuse21", &SceneData->UniformDataInfo.plight[1].diffuse.x, 0.0f, 1.0f);
    ImGui::SliderFloat3("Speculare21", &SceneData->UniformDataInfo.plight[1].specular.x, 0.0f, 1.0f);
    ImGui::SliderFloat("constant1", &SceneData->UniformDataInfo.plight[1].constant, 0.0f, 100.0f);
    ImGui::SliderFloat("linear1", &SceneData->UniformDataInfo.plight[1].linear, 0.0f, 100.0f);
    ImGui::SliderFloat("quadratic1", &SceneData->UniformDataInfo.plight[1].quadratic, 0.0f, 100.0f);

    ImGui::SliderFloat3("Pos22", &SceneData->UniformDataInfo.plight[2].position.x, -10.0f, 10.0f);
    ImGui::SliderFloat3("Ambient22", &SceneData->UniformDataInfo.plight[2].ambient.x, 0.0f, 1.0f);
    ImGui::SliderFloat3("Diffuse22", &SceneData->UniformDataInfo.plight[2].diffuse.x, 0.0f, 1.0f);
    ImGui::SliderFloat3("Speculare22", &SceneData->UniformDataInfo.plight[2].specular.x, 0.0f, 1.0f);
    ImGui::SliderFloat("constant2", &SceneData->UniformDataInfo.plight[2].constant, 0.0f, 100.0f);
    ImGui::SliderFloat("linear2", &SceneData->UniformDataInfo.plight[2].linear, 0.0f, 100.0f);
    ImGui::SliderFloat("quadratic2", &SceneData->UniformDataInfo.plight[2].quadratic, 0.0f, 100.0f);

    ImGui::SliderFloat3("Pos23", &SceneData->UniformDataInfo.plight[3].position.x, -10.0f, 10.0f);
    ImGui::SliderFloat3("Ambient23", &SceneData->UniformDataInfo.plight[3].ambient.x, 0.0f, 1.0f);
    ImGui::SliderFloat3("Diffuse23", &SceneData->UniformDataInfo.plight[3].diffuse.x, 0.0f, 1.0f);
    ImGui::SliderFloat3("Speculare23", &SceneData->UniformDataInfo.plight[3].specular.x, 0.0f, 1.0f);
    ImGui::SliderFloat("constant3", &SceneData->UniformDataInfo.plight[3].constant, 0.0f, 100.0f);
    ImGui::SliderFloat("linear3", &SceneData->UniformDataInfo.plight[3].linear, 0.0f, 100.0f);
    ImGui::SliderFloat("quadratic3", &SceneData->UniformDataInfo.plight[3].quadratic, 0.0f, 100.0f);

    ImGui::SliderFloat3("Pos24", &SceneData->UniformDataInfo.plight[4].position.x, -10.0f, 10.0f);
    ImGui::SliderFloat3("Ambient24", &SceneData->UniformDataInfo.plight[4].ambient.x, 0.0f, 1.0f);
    ImGui::SliderFloat3("Diffuse24", &SceneData->UniformDataInfo.plight[4].diffuse.x, 0.0f, 1.0f);
    ImGui::SliderFloat3("Speculare24", &SceneData->UniformDataInfo.plight[4].specular.x, 0.0f, 1.0f);
    ImGui::SliderFloat("constant4", &SceneData->UniformDataInfo.plight[4].constant, 0.0f, 100.0f);
    ImGui::SliderFloat("linear4", &SceneData->UniformDataInfo.plight[4].linear, 0.0f, 100.0f);
    ImGui::SliderFloat("quadratic4", &SceneData->UniformDataInfo.plight[4].quadratic, 0.0f, 100.0f);
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

    interfaceRenderPass.Draw(engine, imageIndex);
    Update(engine, window, imageIndex);

    if (engine.imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(engine.Device, 1, &engine.imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
    }

    engine.imagesInFlight[imageIndex] = engine.inFlightFences[currentFrame];


    /// <summary>
    /// Draw Area
    /// </summary>

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(RasterCommandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = RenderPass.RenderPass;
    renderPassInfo.framebuffer = RenderPass.SwapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = engine.SwapChain.SwapChainResolution;

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
    clearValues[1].depthStencil = { 1.0f, 0 };

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(RasterCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
   // skybox.Draw(RasterCommandBuffer, renderPassInfo);

    vkCmdBindPipeline(RasterCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, RenderPass.forwardRendereringPipeline->ShaderPipeline);
    vkCmdBindDescriptorSets(RasterCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, RenderPass.forwardRendereringPipeline->ShaderPipelineLayout, 0, 1, &descriptorSets, 0, nullptr);
    
    assetManager.Draw(RasterCommandBuffer, RenderPass.forwardRendereringPipeline);

    vkCmdEndRenderPass(RasterCommandBuffer);
   // AnimationRenderer.Compute(engine, imageIndex);
       frameBufferRenderPass.Draw(engine, RasterCommandBuffer, imageIndex);
    if (vkEndCommandBuffer(RasterCommandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }

   RayRenderer.buildCommandBuffers(engine, assetManager, descriptorSets, imageIndex);

    ///
    ///Draw area
    /// 

    VkSemaphore waitSemaphores[] = { engine.vulkanSemaphores[currentFrame].ImageAcquiredSemaphore };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    std::vector<VkCommandBuffer> CommandBufferSubmitList;

    if (RayTraceSwitch)
    {
        //CommandBufferSubmitList.emplace_back(AnimationRenderer.commandBuffer);
      //  CommandBufferSubmitList.emplace_back(RayRenderer.RayTraceCommandBuffer);
        CommandBufferSubmitList.emplace_back(RasterCommandBuffer);
        CommandBufferSubmitList.emplace_back(interfaceRenderPass.ImGuiCommandBuffers[imageIndex]);
    }
    else
    {
       // CommandBufferSubmitList.emplace_back(AnimationRenderer.commandBuffer);
        CommandBufferSubmitList.emplace_back(RayRenderer.RayTraceCommandBuffer);
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
    assetManager.Delete(engine);
    interfaceRenderPass.Destroy(engine);
    frameBufferRenderPass.Destroy(engine);
   // AnimationRenderer.Destroy(engine);
    RenderPass.Destroy(engine);
    SceneData->Destroy(engine);
    RayRenderer.Destory(engine);

    vkDestroyDescriptorPool(engine.Device, descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(engine.Device, descriptorSetLayout, nullptr);
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
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 0, VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR, 1 });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR, 1 });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL, 1 });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL, assetManager.GetMeshDescriptorCount() });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 4, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL, assetManager.GetMeshDescriptorCount() });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 5, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL, assetManager.GetMeshDescriptorCount() });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 6, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL, assetManager.GetMeshDescriptorCount() });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 7, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL, assetManager.GetMaterialDescriptorCount() });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 8, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_ANY_HIT_BIT_KHR, assetManager.GetTextureBufferDescriptorCount() });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 9, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,  VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR, assetManager.Get3DTextureBufferDescriptorCount() });
    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 10, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_MISS_BIT_KHR, 1 });
    descriptorSetLayout = engine.CreateDescriptorSetLayout(LayoutBindingInfo);
}

void Renderer::SetUpDescriptorSets(VulkanEngine& engine)
{
    descriptorSets = engine.CreateDescriptorSets(descriptorPool, descriptorSetLayout);

    VkWriteDescriptorSetAccelerationStructureKHR AccelerationDescriptorStructure = engine.AddAcclerationStructureBinding(RayRenderer.topLevelAS.handle);
    VkDescriptorImageInfo RayTraceImageDescriptor = engine.AddRayTraceReturnImageDescriptor(VK_IMAGE_LAYOUT_GENERAL, RayRenderer.storageImage->View);
    VkDescriptorBufferInfo SceneDataBufferInfo = engine.AddBufferDescriptor(SceneData->VulkanBufferData);
    std::vector<VkDescriptorBufferInfo> MeshPropertyDataBufferInfo = assetManager.GetMeshPropertiesListDescriptors();
    std::vector<VkDescriptorBufferInfo> VertexBufferInfoList = assetManager.GetVertexBufferListDescriptors();
    std::vector<VkDescriptorBufferInfo> IndexBufferInfoList = assetManager.GetIndexBufferListDescriptors();
    std::vector<VkDescriptorBufferInfo> TransformBufferList = assetManager.GetTransformBufferListDescriptors();
    std::vector<VkDescriptorBufferInfo> MaterialBufferList = assetManager.GetMaterialBufferListDescriptor();
    std::vector<VkDescriptorImageInfo> TextureBufferInfo = assetManager.GetTextureBufferListDescriptor();
    std::vector<VkDescriptorImageInfo> Texture3DBufferInfo = assetManager.Get3DTextureBufferListDescriptor();
    VkDescriptorImageInfo CubeMapImage = assetManager.GetSkyBoxTextureBufferListDescriptor();

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



