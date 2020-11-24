#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define STB_IMAGE_IMPLEMENTATION

#include "VulkanGraphics.h"

VulkanGraphics::VulkanGraphics()
{
    window = VulkanWindow(800, 600, "Vulkan Engine");
    vulkanEngine = VulkanEngine(window.GetWindowPtr());
    renderManager = RenderManager(vulkanEngine, window.GetWindowPtr());
    textureManager = std::make_shared<TextureManager>(vulkanEngine);
    light = LightManager(vulkanEngine, textureManager, renderManager.mainRenderPass.debugLightRenderingPipeline->ShaderPipelineDescriptorLayout, RenderDrawFlags::RenderLightDebug, glm::vec3(0.0f));

    MeshTextures meshTextures;
    meshTextures.DiffuseMap = "C:/Users/dhz/source/repos/VulkanGraphics/VulkanGraphics/texture/container2.png";
    meshTextures.SpecularMap = "C:/Users/dhz/source/repos/VulkanGraphics/VulkanGraphics/texture/container2_specular.png";
    meshTextures.NormalMap = DefaultTexture;
    meshTextures.AlphaMap = DefaultTexture;
    meshTextures.DepthMap = DefaultTexture;
    meshTextures.EmissionMap = DefaultTexture;
    meshTextures.ReflectionMap = DefaultTexture;
    meshTextures.CubeMap[0] = "C:/Users/dhz/source/repos/Vulkan_SkeletonTest/Vulkan_SkeletonTest/VulkanGraphics/texture/skybox/left.jpg";
    meshTextures.CubeMap[1] = "C:/Users/dhz/source/repos/Vulkan_SkeletonTest/Vulkan_SkeletonTest/VulkanGraphics/texture/skybox/right.jpg";
    meshTextures.CubeMap[2] = "C:/Users/dhz/source/repos/Vulkan_SkeletonTest/Vulkan_SkeletonTest/VulkanGraphics/texture/skybox/top.jpg";
    meshTextures.CubeMap[3] = "C:/Users/dhz/source/repos/Vulkan_SkeletonTest/Vulkan_SkeletonTest/VulkanGraphics/texture/skybox/bottom.jpg";
    meshTextures.CubeMap[4] = "C:/Users/dhz/source/repos/Vulkan_SkeletonTest/Vulkan_SkeletonTest/VulkanGraphics/texture/skybox/back.jpg";
    meshTextures.CubeMap[5] = "C:/Users/dhz/source/repos/Vulkan_SkeletonTest/Vulkan_SkeletonTest/VulkanGraphics/texture/skybox/front.jpg";


    camera = std::make_shared<PerspectiveCamera>(PerspectiveCamera(glm::vec2(vulkanEngine.SwapChain.GetSwapChainResolution().width / (float)vulkanEngine.SwapChain.GetSwapChainResolution().height), glm::vec3(0.0f)));

    const std::vector<Vertex> CubeVertices =
    {
        {{-0.5,-0.5,-0.5}, {0,-1,0}, {0,1}},
        {{0.5,-0.5,-0.5}, {0,-1,0}, {1,1}},
        {{0.5,-0.5,0.5}, {0,-1,0}, {1,0}},
        {{-0.5,-0.5,0.5}, {0,-1,0}, {0,0}},
        {{-0.5,0.5,-0.5}, {0,0,-1}, {0,0}},
        {{0.5,0.5,-0.5}, {0,0,-1}, {1,0}},
        {{0.5,-0.5,-0.5}, {0,0,-1}, {1,1}},
        {{-0.5,-0.5,-0.5}, {0,0,-1}, {0,1}},
        {{0.5,0.5,-0.5}, {1,0,0}, {0,0}},
        {{0.5,0.5,0.5}, {1,0,0}, {1,0}},
        {{0.5,-0.5,0.5}, {1,0,0}, {1,1}},
        {{0.5,-0.5,-0.5}, {1,0,0}, {0,1}},
        {{0.5,0.5,0.5}, {0,0,1}, {1,0}},
        {{-0.5,0.5,0.5}, {0,0,1}, {0,0}},
        {{-0.5,-0.5,0.5}, {0,0,1}, {0,1}},
        {{0.5,-0.5,0.5}, {0,0,1}, {1,1}},
        {{-0.5,0.5,0.5}, {-1,0,0}, {1,0}},
        {{-0.5,0.5,-0.5}, {-1,0,0}, {0,0}},
        {{-0.5,-0.5,-0.5}, {-1,0,0}, {0,1}},
        {{-0.5,-0.5,0.5}, {-1,0,0}, {1,1}},
        {{-0.5,0.5,-0.5}, {0,1,0}, {0,1}},
        {{-0.5,0.5,0.5}, {0,1,0}, {0,0}},
        {{0.5,0.5,0.5}, {0,1,0}, {1,0}},
        {{0.5,0.5,-0.5}, {0,1,0}, {1,1}},
    };

    const std::vector<uint16_t> CubeIndices = {
        0,1,2, 0,2,3,
        4,5,6, 4,6,7,
        8,9,10, 8,10,11,
        12,13,14, 12,14,15,
        16,17,18, 16,18,19,
        20,21,22, 20,22,23,
    };

    ModelList.emplace_back(Model(vulkanEngine, textureManager, CubeVertices, CubeIndices, meshTextures, renderManager.mainRenderPass.forwardRendereringPipeline->ShaderPipelineDescriptorLayout, RenderDrawFlags::RenderNormally ));
    ModelList.emplace_back(Model(vulkanEngine, textureManager, "C:/Users/dhz/source/repos/Vulkan_SkeletonTest/Vulkan_SkeletonTest/VulkanGraphics/Models/TestAnimModel/model.dae", renderManager.mainRenderPass.forwardRendereringPipeline->ShaderPipelineDescriptorLayout, RenderDrawFlags::RenderAnimated));

    ModelList[0].ModelPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    ModelList[0].ModelRotation = glm::vec3(0.0f, 0.0f, 90.0f);
    //ModelList[1].ModelPosition = glm::vec3(2.0f, 4.0f, 0.0f);
    //ModelList[2].ModelPosition = glm::vec3(3.0f, 3.0f, 0.0f);
    //ModelList[3].ModelPosition = glm::vec3(7.0f, 3.0f, 0.0f);

    Skybox = SkyBoxMesh(vulkanEngine, textureManager, renderManager.mainRenderPass.skyBoxPipeline->ShaderPipelineDescriptorLayout, meshTextures);
    renderManager.CMDBuffer(vulkanEngine, ModelList, Skybox, light);

    //light.Update(vulkanEngine, camera);
    LightBufferObject lighter = {};
    lighter.dLight.diffuse = glm::vec3(-0.2f, -1.0f, -0.3f);
    lighter.dLight.ambient = glm::vec3(0.05f, 0.05f, 0.05f);
    lighter.dLight.diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
    lighter.dLight.specular = glm::vec3(0.5f, 0.5f, 0.5f);

    lighter.pLight.position = glm::vec3(0.05f, 0.05f, 0.05f);
    lighter.pLight.ambient = glm::vec3(0.05f, 0.05f, 0.05f);
    lighter.pLight.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    lighter.pLight.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    lighter.pLight.constant = 1.0f;
    lighter.pLight.linear = 0.09f;
    lighter.pLight.quadratic = 0.032f;
}

VulkanGraphics::~VulkanGraphics()
{
    renderManager.Destroy(vulkanEngine);
    light.Destory(vulkanEngine);
    for (auto model : ModelList)
    {
        model.Destroy(vulkanEngine);
    }
    Skybox.Destory(vulkanEngine);
    textureManager->UnloadAllTextures(vulkanEngine);
    vulkanEngine.Destory();
    window.CleanUp();
}

void VulkanGraphics::MainLoop()
{
    while (!glfwWindowShouldClose(window.GetWindowPtr())) 
    {
        glfwPollEvents();

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::CheckboxFlags("Normally", &ModelList[0].MeshList[0]->RenderFlags, RenderDrawFlags::RenderNormally);
            ImGui::CheckboxFlags("WireFrame", &ModelList[0].MeshList[0]->RenderFlags, RenderDrawFlags::RenderWireFrame);
            ImGui::CheckboxFlags("Shadow", &ModelList[0].MeshList[0]->RenderFlags, RenderDrawFlags::RenderShadow);

  /*          ImGui::CheckboxFlags("Normally2", &ModelList[1].MeshList[0]->RenderFlags, RenderDrawFlags::RenderNormally);
            ImGui::CheckboxFlags("WireFrame2", &ModelList[1].MeshList[0]->RenderFlags, RenderDrawFlags::RenderWireFrame);
            ImGui::CheckboxFlags("Shadow2", &ModelList[1].MeshList[0]->RenderFlags, RenderDrawFlags::RenderShadow);

            ImGui::CheckboxFlags("Normally3", &ModelList[2].MeshList[0]->RenderFlags, RenderDrawFlags::RenderNormally);
            ImGui::CheckboxFlags("WireFrame3", &ModelList[2].MeshList[0]->RenderFlags, RenderDrawFlags::RenderWireFrame);
            ImGui::CheckboxFlags("Shadow3", &ModelList[2].MeshList[0]->RenderFlags, RenderDrawFlags::RenderShadow);

            ImGui::CheckboxFlags("Normally4", &ModelList[3].MeshList[0]->RenderFlags, RenderDrawFlags::RenderNormally);
            ImGui::CheckboxFlags("WireFrame4", &ModelList[3].MeshList[0]->RenderFlags, RenderDrawFlags::RenderWireFrame);
            ImGui::CheckboxFlags("Shadow4", &ModelList[3].MeshList[0]->RenderFlags, RenderDrawFlags::RenderShadow);*/

           // ImGui::Image(renderManager.gBufferRenderPass.GPositionTexture->ImGuiDescriptorSet, ImVec2(80.0f, 80.0f));
          //  ImGui::Image(renderManager.gBufferRenderPass.GNormalTexture->ImGuiDescriptorSet, ImVec2(80.0f, 80.0f));
          //  ImGui::Image(renderManager.gBufferRenderPass.GAlbedoTexture->ImGuiDescriptorSet, ImVec2(80.0f, 80.0f));
         //   ImGui::Image(renderManager.gBufferRenderPass.BloomTexture->ImGuiDescriptorSet, ImVec2(80.0f, 80.0f));
         //   ImGui::Image(renderManager.shadowRenderPass.DepthTexture->ImGuiDescriptorSet, ImVec2(80.0f, 80.0f));
            
          /*  ImGui::SliderFloat3("dLight", &light.light.dLight.direction.x, -10.0f, 10.0f);
            ImGui::SliderFloat3("dambient", &light.light.dLight.ambient.x, 0.0f, 1.0f);
            ImGui::SliderFloat3("ddiffuse", &light.light.dLight.diffuse.x, 0.0f, 1.0f);
            ImGui::SliderFloat3("dspecular", &light.light.dLight.specular.x, 0.0f, 1.0f);*/

           // ImGui::SliderInt("Using1", &light.PointLightList[0]->pointLight.InUseFlag, 0.0f, 1.0f);
            ImGui::SliderFloat3("pLight", &lighter.dLight.direction.x, -10.0f, 10.0f);
            ImGui::SliderFloat3("pambient", &lighter.dLight.ambient.x, 0.0f, 1.0f);
            ImGui::SliderFloat3("pdiffuse", &lighter.dLight.diffuse.x, 0.0f, 1.0f);
            ImGui::SliderFloat3("pspecular", &lighter.dLight.specular.x, 0.0f, 1.0f);

           // ImGui::SliderInt("Using2", &light.PointLightList[1]->pointLight.InUseFlag, 0.0f, 1.0f);
            ImGui::SliderFloat3("pLight2", &lighter.pLight.position.x, -10.0f, 10.0f);
            ImGui::SliderFloat3("pambient2", &lighter.pLight.ambient.x, 0.0f, 1.0f);
            ImGui::SliderFloat3("pdiffuse2", &lighter.pLight.diffuse.x, 0.0f, 1.0f);
            ImGui::SliderFloat3("pspecular2", &lighter.pLight.specular.x, 0.0f, 1.0f);

            /*ImGui::SliderInt("Using3", &light.PointLightList[2]->pointLight.InUseFlag, 0.0f, 1.0f);
            ImGui::SliderFloat3("pLight3", &light.PointLightList[2]->pointLight.position.x, -100.0f, 100.0f);
            ImGui::SliderFloat3("pambient3", &light.PointLightList[2]->pointLight.ambient.x, 0.0f, 1.0f);
            ImGui::SliderFloat3("pdiffuse3", &light.PointLightList[2]->pointLight.diffuse.x, 0.0f, 1.0f);
            ImGui::SliderFloat3("pspecular3", &light.PointLightList[2]->pointLight.specular.x, 0.0f, 1.0f);

            ImGui::SliderInt("Using4", &light.PointLightList[3]->pointLight.InUseFlag, 0.0f, 1.0f);
            ImGui::SliderFloat3("pLight4", &light.PointLightList[3]->pointLight.position.x, -100.0f, 100.0f);
            ImGui::SliderFloat3("pambient4", &light.PointLightList[3]->pointLight.ambient.x, 0.0f, 1.0f);
            ImGui::SliderFloat3("pdiffuse4", &light.PointLightList[3]->pointLight.diffuse.x, 0.0f, 1.0f);
            ImGui::SliderFloat3("pspecular4", &light.PointLightList[3]->pointLight.specular.x, 0.0f, 1.0f);*/
            
            textureManager->UpdateIMGUIVRAM();
        }
        ImGui::Render();

        //renderManager.UpdateCommandBuffer(vulkanEngine, ModelList, Skybox);
        UpdateUniformBuffer(vulkanEngine.DrawFrame);
        renderManager.Draw(vulkanEngine, window.GetWindowPtr(), ModelList, Skybox, light);
        mouse.Update(window.GetWindowPtr(), camera);
        keyboard.Update(window.GetWindowPtr(), camera);
    }

    vkDeviceWaitIdle(vulkanEngine.Device);
}

void VulkanGraphics::UpdateUniformBuffer(uint32_t currentImage)
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    camera->Update(vulkanEngine);


    lighter.viewPos = camera->GetPosition();
    for (auto& model : ModelList)
    {
        model.Update(vulkanEngine, camera, lighter);
    }
    Skybox.UpdateUniformBuffer(vulkanEngine, camera);
}
