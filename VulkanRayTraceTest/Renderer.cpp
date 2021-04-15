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
    interfaceRenderPass = InterfaceRenderPass(engine, window.GetWindowPtr());
    assetManager = AssetManager(engine);
    SceneData = std::make_shared<SceneDataUniformBuffer>(SceneDataUniformBuffer(engine));
    assetManager.meshManager.MeshList.emplace_back(std::make_shared<MegaMan>(MegaMan(engine, assetManager, glm::vec3(1.0f, 0.0f, 0.0f))));
    //assetManager.meshManager.MeshList.emplace_back(std::make_shared<MegaMan>(MegaMan(engine, assetManager, glm::vec3(1.0f, 0.0f, 0.0f))));
    //assetManager.meshManager.MeshList.emplace_back(std::make_shared<MegaMan>(MegaMan(engine, assetManager, glm::vec3(2.0f, 0.0f, 0.0f))));
    //assetManager.meshManager.MeshList.emplace_back(std::make_shared<MegaMan>(MegaMan(engine, assetManager, glm::vec3(3.0f, 0.0f, 0.0f))));
    //assetManager.meshManager.MeshList.emplace_back(std::make_shared<Mario>(Mario(engine, assetManager, glm::vec3(4.0f, 0.0f, 0.0f))));

    assetManager.AddModel();
    assetManager.modelManager.ModelList.back()->AddMesh(engine, assetManager.meshManager.MeshList[0]);
    //assetManager.modelManager.ModelList.back()->AddMesh(engine, assetManager.meshManager.MeshList[1]);
    //assetManager.modelManager.ModelList.back()->AddMesh(engine, assetManager.meshManager.MeshList[2]);
    //assetManager.modelManager.ModelList.back()->AddMesh(engine, assetManager.meshManager.MeshList[3]);
    
  //  assetManager.AddModel(engine, "../Models/vulkanscene_shadow.obj");
    //std::shared_ptr<Material> material = std::make_shared<Material>(engine, assetManager.textureManager);
    //material->materialTexture.AlbedoMap = assetManager.textureManager.LoadTexture2D(engine, "../texture/pbr/rusted_iron/albedo.png", VK_FORMAT_R8G8B8A8_SRGB);
    //material->materialTexture.NormalMap = assetManager.textureManager.LoadTexture2D(engine, "../texture/pbr/rusted_iron/normal.png", VK_FORMAT_R8G8B8A8_UNORM);
    //material->materialTexture.RoughnessMap = assetManager.textureManager.LoadTexture2D(engine, "../texture/pbr/rusted_iron/roughness.png", VK_FORMAT_R8G8B8A8_UNORM);
    //material->materialTexture.AOMap = assetManager.textureManager.LoadTexture2D(engine, "../texture/pbr/rusted_iron/ao.png", VK_FORMAT_R8G8B8A8_UNORM);
    //material->materialTexture.MatallicMap = assetManager.textureManager.LoadTexture2D(engine, "../texture/pbr/rusted_iron/metallic.png", VK_FORMAT_R8G8B8A8_UNORM);
    //uint32_t MaterialID = assetManager.materialManager.LoadMaterial(engine, "MarioMaterial", material);

    assetManager.AddModel(engine, "../Models/RayReflectionTest.obj");

    std::string CubeMapFiles[6];
    CubeMapFiles[0] = "../texture/skybox/right.jpg";
    CubeMapFiles[1] = "../texture/skybox/left.jpg";
    CubeMapFiles[2] = "../texture/skybox/top.jpg";
    CubeMapFiles[3] = "../texture/skybox/bottom.jpg";
    CubeMapFiles[4] = "../texture/skybox/back.jpg";
    CubeMapFiles[5] = "../texture/skybox/front.jpg";

    assetManager.textureManager.LoadCubeMap(engine, CubeMapFiles, VK_FORMAT_R8G8B8A8_SRGB);

    RenderPass = ForwardRenderPass(engine, assetManager, SceneData);
    SetUpCommandBuffers(engine);

  //  frameBufferRenderPass = FrameBufferRenderPass(engine, assetManager, SceneData);
    //waterRenderPass = WaterRenderPass(engine, assetManager, SceneData);
    //gBufferRenderPass = GBufferRenderPass(engine, assetManager, SceneData);
    //textureRenderPass = TextureRenderPass(engine, assetManager, SceneData);
    skybox = Skybox(engine, assetManager, RenderPass.RenderPass);

    camera = std::make_shared<PerspectiveCamera>(glm::vec2(engine.SwapChain.SwapChainResolution.width, engine.SwapChain.SwapChainResolution.height), glm::vec3(0.0f, 0.0f, 5.0f));
    camera2 = std::make_shared<PerspectiveCamera>(glm::vec2(engine.SwapChain.SwapChainResolution.width, engine.SwapChain.SwapChainResolution.height), glm::vec3(0.0f, 0.0f, 5.0f));

    //assetManager.modelManager.ModelList[0]->MeshList[0]->MaterialID = MaterialID;


      //assetManager.AddModel(engine, "../Models/TestAnimModel/model.dae");
      //assetManager.AddModel(engine, "../Models/cyborg/cyborg.obj");
      // modelRenderManager.AddModel(engine, "../Models/Sponza/Sponza.obj");


      //RayRenderer = RayTraceRenderer(engine, assetManager, SceneData);
      //AnimationRenderer = AnimatorCompute(engine, assetManager.modelManager.ModelList[2]->MeshList[0]);

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

    
   // auto a =    assetManager.textureManager.LoadTexture2D(gBufferRenderPass.GPositionTexture);
//auto b = assetManager.textureManager.LoadTexture2D(gBufferRenderPass.GNormalTexture);
//auto c = assetManager.textureManager.LoadTexture2D(gBufferRenderPass.GAlbedoTexture);
//auto d = assetManager.textureManager.LoadTexture2D(gBufferRenderPass.GBloomTexture);
//auto e = assetManager.textureManager.LoadTexture2D(gBufferRenderPass.DepthTexture);
//auto f = assetManager.textureManager.LoadTexture2D(RayRenderer.storageImage);
    RenderPass.UpdateSwapChain(engine, assetManager, SceneData);

    //ImGui_ImplVulkan_AddTexture(assetManager.textureManager.TextureList[0]->ImGuiDescriptorSet, assetManager.textureManager.TextureList[0]->Sampler, assetManager.textureManager.TextureList[0]->View, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    //ImGui_ImplVulkan_AddTexture(gBufferRenderPass.GBloomTexture->ImGuiDescriptorSet, gBufferRenderPass.GBloomTexture->Sampler, gBufferRenderPass.GBloomTexture->View, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    //ImGui_ImplVulkan_AddTexture(gBufferRenderPass.GNormalTexture->ImGuiDescriptorSet, gBufferRenderPass.GNormalTexture->Sampler, gBufferRenderPass.GNormalTexture->View, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    //ImGui_ImplVulkan_AddTexture(waterRenderPass.ReflectionTexture->ImGuiDescriptorSet, waterRenderPass.ReflectionTexture->Sampler, waterRenderPass.ReflectionTexture->View, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    //ImGui_ImplVulkan_AddTexture(waterRenderPass.RefractionTexture->ImGuiDescriptorSet, waterRenderPass.RefractionTexture->Sampler, waterRenderPass.RefractionTexture->View, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
   ///./ ImGui_ImplVulkan_AddTexture(RayRenderer.storageImage->ImGuiDescriptorSet, RayRenderer.storageImage->Sampler, RayRenderer.storageImage->View, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
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

    engine.SwapChain.UpdateSwapChain(window.GetWindowPtr(), engine.Device, engine.PhysicalDevice, engine.Surface);
    //gBufferRenderPass.UpdateSwapChain(engine, assetManager, SceneData);
    RenderPass.UpdateSwapChain(engine, assetManager, SceneData);
    //frameBufferRenderPass.UpdateSwapChain(engine, assetManager, SceneData);
    interfaceRenderPass.UpdateSwapChain(engine);
    //waterRenderPass.UpdateSwapChain(engine, assetManager, SceneData);
   // textureRenderPass.UpdateSwapChain(engine, assetManager, SceneData);
 //  RayRenderer.Resize(engine, assetManager, SceneData, 0);

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
    camera2->Update(engine);

    assetManager.Update(engine);
    skybox.Update(engine, assetManager.materialManager, camera);
   // RayRenderer.createTopLevelAccelerationStructure(engine, assetManager);

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

    //WaterRenderPass.Update(engine, assetManager, *SceneData.get(), camera2);
    //waterRenderPass.Update(engine, assetManager, *SceneData.get());
}

void Renderer::GUIUpdate(VulkanEngine& engine)
{

    for (int y = 0; y < assetManager.materialManager.MaterialList.size(); y++)
    {
        auto a = std::to_string(y);;
        ImGui::Text(("Material  " + std::to_string(y)).c_str());
      //  ImGui::SliderFloat3(("Albedo  " + std::to_string(y)).c_str(), &assetManager.materialManager.MaterialList[y]->materialTexture.Albedo.x, 0.0f, 1.0f);
        //ImGui::SliderFloat(("Roughness  " + std::to_string(y)).c_str(), &assetManager.materialManager.MaterialList[y]->materialTexture.Roughness, 0.0f, 1.0f);
        //ImGui::SliderFloat(("Metallic  " + std::to_string(y)).c_str(), &assetManager.materialManager.MaterialList[y]->materialTexture.Matallic, 0.0f, 1.0f);
        ImGui::SliderFloat(("AO  " + std::to_string(y)).c_str(), &assetManager.materialManager.MaterialList[y]->materialTexture.Reflectivness, 0.0f, 1.0f);
       
    }

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::SliderFloat("TextureIndex", &SceneData->UniformDataInfo.temp2, 0, 1.0f);
    ImGui::Checkbox("RayTraceSwitch", &RayTraceSwitch);
    //ImGui::SliderInt("Shadow", &SceneData->UniformDataInfo.Shadowed, 0, 1);
    //ImGui::Checkbox("AddTexture", &AddTextureFlag);
    //ImGui::Checkbox("DeleteTexture", &RemoveTextureFlag);
    //ImGui::Checkbox("AddMaterial", &AddMaterialFlag);
    //ImGui::Checkbox("DeleteMaterial", &RemoveMaterialFlag);

   // ImGui::Image(RayRenderer.storageImage->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
   // ImGui::SliderFloat3("ReflectCamPos", &waterRenderPass.ReflectionCam->Position.x, -10.0f, 10.0f);
    //ImGui::Image(waterRenderPass.ReflectionTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
   // ImGui::Image(waterRenderPass.RefractionTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    //ImGui::Image(gBufferRenderPass.GBloomTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    //ImGui::Image(gBufferRenderPass.GNormalTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    //ImGui::Image(gBufferRenderPass.GPositionTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
    //for (auto& texture : assetManager.textureManager.TextureList)
    //{
    //    auto bufferIndex = std::to_string(texture->TextureBufferIndex);
    //    ImGui::LabelText(bufferIndex.c_str(), "sd");
    //    ImGui::Image(texture->ImGuiDescriptorSet, ImVec2(512, 512));
    //}

    //ImGui::SliderFloat3("Position2", &SceneData->UniformDataInfo.sLight.position.x, 0.0f, 10.0f);
    //ImGui::SliderFloat3("Direction2", &SceneData->UniformDataInfo.sLight.direction.x, -1.0f, 1.0f);
    //ImGui::SliderFloat3("Ambient245", &SceneData->UniformDataInfo.sLight.ambient.x, 0.0f, 1.0f);
    //ImGui::SliderFloat3("Diffuse245", &SceneData->UniformDataInfo.sLight.diffuse.x, 0.0f, 1.0f);
    //ImGui::SliderFloat3("Speculare245", &SceneData->UniformDataInfo.sLight.specular.x, 0.0f, 1.0f);
    //ImGui::SliderFloat("constant45", &SceneData->UniformDataInfo.sLight.constant, 0.0f, 100.0f);
    //ImGui::SliderFloat("linear45", &SceneData->UniformDataInfo.sLight.linear, 0.0f, 100.0f);
    //ImGui::SliderFloat("quadratic45", &SceneData->UniformDataInfo.sLight.quadratic, 0.0f, 100.0f);
    //ImGui::SliderFloat("quadratic452", &SceneData->UniformDataInfo.sLight.cutOff, 0.0f, 100.0f);
    //ImGui::SliderFloat("quadratic453", &SceneData->UniformDataInfo.sLight.outerCutOff, 0.0f, 100.0f);

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

    Update(engine, window, imageIndex);

    if (engine.imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(engine.Device, 1, &engine.imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
    }

    engine.imagesInFlight[imageIndex] = engine.inFlightFences[currentFrame];


    /// <summary>
    /// Draw Area
    /// </summary>
   //gBufferRenderPass.Draw(engine, assetManager, imageIndex);
   RenderPass.Draw(engine, assetManager, imageIndex, RasterCommandBuffer, skybox);
   //waterRenderPass.Draw(engine, assetManager, imageIndex);
   //WaterRenderPass.Draw(engine, assetManager, imageIndex);
   //frameBufferRenderPass.Draw(engine, RasterCommandBuffer, imageIndex);
 //  RayRenderer.buildCommandBuffers(engine, assetManager, imageIndex);
   interfaceRenderPass.Draw(engine, imageIndex);
    ///
    ///Draw area
    /// 

    VkSemaphore waitSemaphores[] = { engine.vulkanSemaphores[currentFrame].ImageAcquiredSemaphore };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    std::vector<VkCommandBuffer> CommandBufferSubmitList;

    if (RayTraceSwitch)
    {
        //CommandBufferSubmitList.emplace_back(AnimationRenderer.commandBuffer);
       // CommandBufferSubmitList.emplace_back(RayRenderer.RayTraceCommandBuffer);
       // CommandBufferSubmitList.emplace_back(gBufferRenderPass.CommandBuffer);
        CommandBufferSubmitList.emplace_back(RasterCommandBuffer);
        //CommandBufferSubmitList.emplace_back(waterRenderPass.CommandBuffer);
        //CommandBufferSubmitList.emplace_back(WaterRenderPass.CommandBuffer);
        CommandBufferSubmitList.emplace_back(interfaceRenderPass.ImGuiCommandBuffers[imageIndex]);
    }
    else
    {
       // CommandBufferSubmitList.emplace_back(gBufferRenderPass.CommandBuffer);
       // CommandBufferSubmitList.emplace_back(AnimationRenderer.commandBuffer);
       // CommandBufferSubmitList.emplace_back(RayRenderer.RayTraceCommandBuffer);
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

    if (AddTextureFlag == true)
    {
        vkDeviceWaitIdle(engine.Device);
        assetManager.textureManager.LoadTexture2D(engine, "../texture/toy_box_diffuse.png", VK_FORMAT_R8G8B8A8_UNORM);

        UpdateSwapChain(engine, window);
        AddTextureFlag = false;
    }

    if (RemoveTextureFlag == true)
    {
        vkDeviceWaitIdle(engine.Device);

        assetManager.textureManager.DeleteTexture(engine, SceneData->UniformDataInfo.temp);

        UpdateSwapChain(engine, window);
        RemoveTextureFlag = false;
    }

    if (AddMaterialFlag == true)
    {
        vkDeviceWaitIdle(engine.Device);

        stbi_set_flip_vertically_on_load(true);
        std::shared_ptr<Material> material = std::make_shared<Material>(engine, assetManager.textureManager);
        material->materialTexture.DiffuseMap = assetManager.textureManager.LoadTexture2D(engine, "../texture/toy_box_diffuse.png", VK_FORMAT_R8G8B8A8_UNORM);
        material->materialTexture.NormalMap = assetManager.textureManager.LoadTexture2D(engine, "../texture/toy_box_normal.png", VK_FORMAT_R8G8B8A8_UNORM);
        material->materialTexture.DepthMap = assetManager.textureManager.LoadTexture2D(engine, "../texture/toy_box_disp.png", VK_FORMAT_R8G8B8A8_UNORM);
        assetManager.materialManager.LoadMaterial(engine, "zdsf", material);
        stbi_set_flip_vertically_on_load(false);

        UpdateSwapChain(engine, window);
        AddMaterialFlag = false;
    }

    if (RemoveMaterialFlag == true)
    {
        vkDeviceWaitIdle(engine.Device);

        assetManager.materialManager.DeleteMaterial(engine, SceneData->UniformDataInfo.temp);

        UpdateSwapChain(engine, window);
        RemoveMaterialFlag = false;
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::Destroy(VulkanEngine& engine)
{
    assetManager.Delete(engine);
    interfaceRenderPass.Destroy(engine);
   // frameBufferRenderPass.Destroy(engine);
    //waterRenderPass.Destroy(engine);
    //WaterRenderPass.Destroy(engine);
    //gBufferRenderPass.Destroy(engine);
    // AnimationRenderer.Destroy(engine);
    RenderPass.Destroy(engine);
    SceneData->Destroy(engine);
  //  RayRenderer.Destory(engine);
}
