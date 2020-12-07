#include "Game.h"
#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <stb_image.h>
#define STB_IMAGE_IMPLEMENTATION

Game::Game()
{

    ////DDS_HEADER header;
    ////std::ifstream file2("C:/Users/dotha/source/repos/VulkanGraphics/VulkanGraphics/texture/skybox/front.dds", std::ios::binary);
    ////file2.read(reinterpret_cast<char*>(&header), sizeof(header));

    //window = VulkanWindow(800, 600, "Vulkan Engine");
    //vulkanEngine = VulkanEngine(window.GetWindowPtr());
    //renderManager = RenderManager(vulkanEngine, window.GetWindowPtr());
    //textureManager = std::make_shared<TextureManager>(vulkanEngine);
    //light = LightManager(vulkanEngine, textureManager, renderManager.mainRenderPass.debugLightRenderingPipeline->ShaderPipelineDescriptorLayout, RenderDrawFlags::RenderLightDebug, glm::vec3(0.0f));

    ////std::vector<Pixel> pixels(800 * 600);
    ////std::uniform_real_distribution<float> randomFloats(0, 255); // generates random floats between 0.0 and 1.0
    ////std::default_random_engine generator;

    ////for (int x = 0; x < 800; x++)
    ////{
    ////    for (int y = 0; y < 600; y++)
    ////    {
    ////        byte color = (byte)randomFloats(generator);
    ////        pixels[x + (y * x)].Red = color;
    ////        pixels[x + (y * x)].Green = color;
    ////        pixels[x + (y * x)].Blue = color;
    ////        pixels[x + (y * x)].Alpha = 255;
    ////    }
    ////}
    ////std::shared_ptr<Texture2D> texture = std::make_shared<Texture2D>(vulkanEngine, 800, 600, pixels, TextureType::vkTexture2D, VK_FORMAT_R8G8B8A8_UNORM);
    ////textureManager->LoadTexture(texture);

    //MeshTextures meshTextures;
    //meshTextures.DiffuseMap = "C:/Users/dotha/source/repos/VulkanGraphics/VulkanGraphics/texture/bricks2.jpg";
    //meshTextures.SpecularMap = "C:/Users/dotha/source/repos/VulkanGraphics/VulkanGraphics/texture/container2_specular.png";
    //meshTextures.NormalMap = "C:/Users/dotha/source/repos/VulkanGraphics/VulkanGraphics/texture/bricks2_normal.jpg";
    //meshTextures.AlphaMap = DefaultTexture;
    //meshTextures.DepthMap = "C:/Users/dotha/source/repos/VulkanGraphics/VulkanGraphics/texture/bricks2_disp.jpg";
    //meshTextures.EmissionMap = DefaultTexture;
    //meshTextures.ReflectionMap = DefaultTexture;
    //meshTextures.CubeMap[0] = "C:/Users/dotha/source/repos/VulkanGraphics/VulkanGraphics/texture/skybox/left.jpg";
    //meshTextures.CubeMap[1] = "C:/Users/dotha/source/repos/VulkanGraphics/VulkanGraphics/texture/skybox/right.jpg";
    //meshTextures.CubeMap[2] = "C:/Users/dotha/source/repos/VulkanGraphics/VulkanGraphics/texture/skybox/top.jpg";
    //meshTextures.CubeMap[3] = "C:/Users/dotha/source/repos/VulkanGraphics/VulkanGraphics/texture/skybox/bottom.jpg";
    //meshTextures.CubeMap[4] = "C:/Users/dotha/source/repos/VulkanGraphics/VulkanGraphics/texture/skybox/back.jpg";
    //meshTextures.CubeMap[5] = "C:/Users/dotha/source/repos/VulkanGraphics/VulkanGraphics/texture/skybox/front.jpg";


    //camera = std::make_shared<PerspectiveCamera>(PerspectiveCamera(glm::vec2(vulkanEngine.SwapChain.GetSwapChainResolution().width / (float)vulkanEngine.SwapChain.GetSwapChainResolution().height), glm::vec3(0.0f)));

    //const std::vector<Vertex> CubeVertices =
    //{
    //    {{-0.5,-0.5,-0.5}, {0,-1,0}, {0,1}},
    //    {{0.5,-0.5,-0.5}, {0,-1,0}, {1,1}},
    //    {{0.5,-0.5,0.5}, {0,-1,0}, {1,0}},
    //    {{-0.5,-0.5,0.5}, {0,-1,0}, {0,0}},
    //    {{-0.5,0.5,-0.5}, {0,0,-1}, {0,0}},
    //    {{0.5,0.5,-0.5}, {0,0,-1}, {1,0}},
    //    {{0.5,-0.5,-0.5}, {0,0,-1}, {1,1}},
    //    {{-0.5,-0.5,-0.5}, {0,0,-1}, {0,1}},
    //    {{0.5,0.5,-0.5}, {1,0,0}, {0,0}},
    //    {{0.5,0.5,0.5}, {1,0,0}, {1,0}},
    //    {{0.5,-0.5,0.5}, {1,0,0}, {1,1}},
    //    {{0.5,-0.5,-0.5}, {1,0,0}, {0,1}},
    //    {{0.5,0.5,0.5}, {0,0,1}, {1,0}},
    //    {{-0.5,0.5,0.5}, {0,0,1}, {0,0}},
    //    {{-0.5,-0.5,0.5}, {0,0,1}, {0,1}},
    //    {{0.5,-0.5,0.5}, {0,0,1}, {1,1}},
    //    {{-0.5,0.5,0.5}, {-1,0,0}, {1,0}},
    //    {{-0.5,0.5,-0.5}, {-1,0,0}, {0,0}},
    //    {{-0.5,-0.5,-0.5}, {-1,0,0}, {0,1}},
    //    {{-0.5,-0.5,0.5}, {-1,0,0}, {1,1}},
    //    {{-0.5,0.5,-0.5}, {0,1,0}, {0,1}},
    //    {{-0.5,0.5,0.5}, {0,1,0}, {0,0}},
    //    {{0.5,0.5,0.5}, {0,1,0}, {1,0}},
    //    {{0.5,0.5,-0.5}, {0,1,0}, {1,1}},
    //};

    //const std::vector<uint16_t> CubeIndices = {
    //    0,1,2, 0,2,3,
    //    4,5,6, 4,6,7,
    //    8,9,10, 8,10,11,
    //    12,13,14, 12,14,15,
    //    16,17,18, 16,18,19,
    //    20,21,22, 20,22,23,
    //};

    //ModelList.emplace_back(Model(vulkanEngine, textureManager, CubeVertices, CubeIndices, meshTextures, renderManager.mainRenderPass.forwardRendereringPipeline->ShaderPipelineDescriptorLayout, RenderDrawFlags::RenderNormally | RenderDrawFlags::RenderShadow));
    //ModelList.emplace_back(Model(vulkanEngine, textureManager, CubeVertices, CubeIndices, meshTextures, renderManager.mainRenderPass.forwardRendereringPipeline->ShaderPipelineDescriptorLayout, RenderDrawFlags::RenderNormally | RenderDrawFlags::RenderShadow));
    //ModelList.emplace_back(Model(vulkanEngine, textureManager, CubeVertices, CubeIndices, meshTextures, renderManager.mainRenderPass.forwardRendereringPipeline->ShaderPipelineDescriptorLayout, RenderDrawFlags::RenderNormally | RenderDrawFlags::RenderShadow));
    //ModelList.emplace_back(Model(vulkanEngine, textureManager, CubeVertices, CubeIndices, meshTextures, renderManager.mainRenderPass.forwardRendereringPipeline->ShaderPipelineDescriptorLayout, RenderDrawFlags::RenderNormally | RenderDrawFlags::RenderShadow));
    //ModelList.emplace_back(Model(vulkanEngine, textureManager, CubeVertices, CubeIndices, meshTextures, renderManager.mainRenderPass.forwardRendereringPipeline->ShaderPipelineDescriptorLayout, RenderDrawFlags::RenderNormally | RenderDrawFlags::RenderShadow));

    //ModelList.emplace_back(Model(vulkanEngine, textureManager, CubeVertices, CubeIndices, meshTextures, renderManager.mainRenderPass.forwardRendereringPipeline->ShaderPipelineDescriptorLayout, RenderDrawFlags::RenderNormally | RenderDrawFlags::RenderShadow));
    //ModelList.emplace_back(Model(vulkanEngine, textureManager, CubeVertices, CubeIndices, meshTextures, renderManager.mainRenderPass.forwardRendereringPipeline->ShaderPipelineDescriptorLayout, RenderDrawFlags::RenderNormally | RenderDrawFlags::RenderShadow));
    //ModelList.emplace_back(Model(vulkanEngine, textureManager, CubeVertices, CubeIndices, meshTextures, renderManager.mainRenderPass.forwardRendereringPipeline->ShaderPipelineDescriptorLayout, RenderDrawFlags::RenderNormally | RenderDrawFlags::RenderShadow));
    //ModelList.emplace_back(Model(vulkanEngine, textureManager, CubeVertices, CubeIndices, meshTextures, renderManager.mainRenderPass.forwardRendereringPipeline->ShaderPipelineDescriptorLayout, RenderDrawFlags::RenderNormally | RenderDrawFlags::RenderShadow));
    //ModelList.emplace_back(Model(vulkanEngine, textureManager, CubeVertices, CubeIndices, meshTextures, renderManager.mainRenderPass.forwardRendereringPipeline->ShaderPipelineDescriptorLayout, RenderDrawFlags::RenderNormally | RenderDrawFlags::RenderShadow));

    //ModelList.emplace_back(Model(vulkanEngine, textureManager, "C:/Users/dotha/source/repos/VulkanGraphics/VulkanGraphics/Models/TestAnimModel/model.dae", renderManager.mainRenderPass.forwardRendereringPipeline->ShaderPipelineDescriptorLayout, RenderDrawFlags::RenderAnimated | RenderDrawFlags::RenderShadowAnimated));

    //glm::vec3 cubePositions[] = {
    //glm::vec3(0.0f,  0.0f,  0.0f),
    //glm::vec3(2.0f,  5.0f, -15.0f),
    //glm::vec3(-1.5f, -2.2f, -2.5f),
    //glm::vec3(-3.8f, -2.0f, -12.3f),
    //glm::vec3(2.4f, -0.4f, -3.5f),
    //glm::vec3(-1.7f,  3.0f, -7.5f),
    //glm::vec3(1.3f, -2.0f, -2.5f),
    //glm::vec3(1.5f,  2.0f, -2.5f),
    //glm::vec3(1.5f,  0.2f, -1.5f),
    //glm::vec3(-1.3f,  1.0f, -1.5f)
    //};

    //for (int x = 0; x < 10; x++)
    //{
    //    ModelList[x].ModelPosition = cubePositions[x];
    //}
    ////ModelList[0].ModelRotation = glm::vec3(0.0f, 0.0f, 90.0f);
    ////ModelList[1].ModelPosition = glm::vec3(2.0f, 4.0f, 0.0f);
    ////ModelList[2].ModelPosition = glm::vec3(3.0f, 3.0f, 0.0f);
    ////ModelList[3].ModelPosition = glm::vec3(7.0f, 3.0f, 0.0f);

    //Skybox = SkyBoxMesh(vulkanEngine, textureManager, renderManager.mainRenderPass.skyBoxPipeline->ShaderPipelineDescriptorLayout, meshTextures);
    //renderManager.CMDBuffer(vulkanEngine, camera, ModelList, Skybox, light);
}

Game::~Game()
{
  /*  renderManager.Destroy(vulkanEngine);
    light.Destory(vulkanEngine);
    for (auto model : ModelList)
    {
        model.Destroy(vulkanEngine);
    }
    Skybox.Destory(vulkanEngine);
    textureManager->UnloadAllTextures(vulkanEngine);
    vulkanEngine.Destory();
    window.CleanUp();*/
}

void Game::MainLoop()
{
    //while (!glfwWindowShouldClose(window.GetWindowPtr()))
    //{
    //    glfwPollEvents();

    //    ImGui_ImplVulkan_NewFrame();
    //    ImGui_ImplGlfw_NewFrame();
    //    ImGui::NewFrame();
    //    {
    //        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    //        ImGui::CheckboxFlags("Normally", &ModelList[0].MeshList[0]->RenderFlags, RenderDrawFlags::RenderNormally);
    //        ImGui::CheckboxFlags("WireFrame", &ModelList[0].MeshList[0]->RenderFlags, RenderDrawFlags::RenderWireFrame);
    //        ImGui::CheckboxFlags("Shadow", &ModelList[0].MeshList[0]->RenderFlags, RenderDrawFlags::RenderShadow);

    //        /*          ImGui::CheckboxFlags("Normally2", &ModelList[1].MeshList[0]->RenderFlags, RenderDrawFlags::RenderNormally);
    //                  ImGui::CheckboxFlags("WireFrame2", &ModelList[1].MeshList[0]->RenderFlags, RenderDrawFlags::RenderWireFrame);
    //                  ImGui::CheckboxFlags("Shadow2", &ModelList[1].MeshList[0]->RenderFlags, RenderDrawFlags::RenderShadow);

    //                  ImGui::CheckboxFlags("Normally3", &ModelList[2].MeshList[0]->RenderFlags, RenderDrawFlags::RenderNormally);
    //                  ImGui::CheckboxFlags("WireFrame3", &ModelList[2].MeshList[0]->RenderFlags, RenderDrawFlags::RenderWireFrame);
    //                  ImGui::CheckboxFlags("Shadow3", &ModelList[2].MeshList[0]->RenderFlags, RenderDrawFlags::RenderShadow);

    //                  ImGui::CheckboxFlags("Normally4", &ModelList[3].MeshList[0]->RenderFlags, RenderDrawFlags::RenderNormally);
    //                  ImGui::CheckboxFlags("WireFrame4", &ModelList[3].MeshList[0]->RenderFlags, RenderDrawFlags::RenderWireFrame);
    //                  ImGui::CheckboxFlags("Shadow4", &ModelList[3].MeshList[0]->RenderFlags, RenderDrawFlags::RenderShadow);*/

    //        ImGui::Image(renderManager.gBufferRenderPass.SSAOTexture->ImGuiDescriptorSet, ImVec2(80.0f, 80.0f));
    //        //ImGui::Image(renderManager.gBufferRenderPass.SSAOBlurTexture->ImGuiDescriptorSet, ImVec2(80.0f, 80.0f));
    //        ImGui::Image(renderManager.gBufferRenderPass.GPositionTexture->ImGuiDescriptorSet, ImVec2(80.0f, 80.0f));
    //        ImGui::Image(renderManager.gBufferRenderPass.GNormalTexture->ImGuiDescriptorSet, ImVec2(80.0f, 80.0f));
    //        ImGui::Image(renderManager.gBufferRenderPass.GAlbedoTexture->ImGuiDescriptorSet, ImVec2(80.0f, 80.0f));
    //        ImGui::Image(renderManager.gBufferRenderPass.BloomTexture->ImGuiDescriptorSet, ImVec2(80.0f, 80.0f));
    //        ImGui::Image(renderManager.shadowRenderPass.DebugColorTexture->ImGuiDescriptorSet, ImVec2(80.0f, 80.0f));

    //        ImGui::SliderFloat3("dLight", &light.light.dLight.direction.x, -10.0f, 10.0f);
    //        ImGui::SliderFloat3("dambient", &light.light.dLight.ambient.x, 0.0f, 1.0f);
    //        ImGui::SliderFloat3("ddiffuse", &light.light.dLight.diffuse.x, 0.0f, 1.0f);
    //        ImGui::SliderFloat3("dspecular", &light.light.dLight.specular.x, 0.0f, 1.0f);

    //        ImGui::SliderInt("Using1", &light.light.pLight[0].InUseFlag, 0.0f, 1.0f);
    //        ImGui::SliderFloat3("pLight", &light.light.pLight[0].position.x, -10.0f, 10.0f);
    //        ImGui::SliderFloat3("pambient", &light.light.pLight[0].ambient.x, 0.0f, 1.0f);
    //        ImGui::SliderFloat3("pdiffuse", &light.light.pLight[0].diffuse.x, 0.0f, 1.0f);
    //        ImGui::SliderFloat3("pspecular", &light.light.pLight[0].specular.x, 0.0f, 1.0f);

    //        ImGui::SliderInt("Using2", &light.light.pLight[1].InUseFlag, 0.0f, 1.0f);
    //        ImGui::SliderFloat3("pLight2", &light.light.pLight[1].position.x, -10.0f, 10.0f);
    //        ImGui::SliderFloat3("pambient2", &light.light.pLight[1].ambient.x, 0.0f, 1.0f);
    //        ImGui::SliderFloat3("pdiffuse2", &light.light.pLight[1].diffuse.x, 0.0f, 1.0f);
    //        ImGui::SliderFloat3("pspecular2", &light.light.pLight[1].specular.x, 0.0f, 1.0f);

    //        ImGui::SliderInt("Using3", &light.light.pLight[2].InUseFlag, 0.0f, 1.0f);
    //        ImGui::SliderFloat3("pLight3", &light.light.pLight[2].position.x, -10.0f, 10.0f);
    //        ImGui::SliderFloat3("pambient3", &light.light.pLight[2].ambient.x, 0.0f, 1.0f);
    //        ImGui::SliderFloat3("pdiffuse3", &light.light.pLight[2].diffuse.x, 0.0f, 1.0f);
    //        ImGui::SliderFloat3("pspecular3", &light.light.pLight[2].specular.x, 0.0f, 1.0f);

    //        ImGui::SliderInt("Using4", &light.light.pLight[3].InUseFlag, 0.0f, 1.0f);
    //        ImGui::SliderFloat3("pLight4", &light.light.pLight[3].position.x, -10.0f, 10.0f);
    //        ImGui::SliderFloat3("pambient4", &light.light.pLight[3].ambient.x, 0.0f, 1.0f);
    //        ImGui::SliderFloat3("pdiffuse4", &light.light.pLight[3].diffuse.x, 0.0f, 1.0f);
    //        ImGui::SliderFloat3("pspecular4", &light.light.pLight[3].specular.x, 0.0f, 1.0f);

    //        textureManager->UpdateIMGUIVRAM();
    //    }
    //    ImGui::Render();

    //    //renderManager.UpdateCommandBuffer(vulkanEngine, ModelList, Skybox);
    //    UpdateUniformBuffer(vulkanEngine.DrawFrame);
    //    renderManager.Draw(vulkanEngine, window.GetWindowPtr(), camera, ModelList, Skybox, light);
    //    mouse.Update(window.GetWindowPtr(), camera);
    //    keyboard.Update(window.GetWindowPtr(), camera);
    //}

    //vkDeviceWaitIdle(vulkanEngine.Device);
}

void Game::UpdateUniformBuffer(uint32_t currentImage)
{
    //light.Update(vulkanEngine, camera);
    //camera->Update(vulkanEngine);

    //for (auto& model : ModelList)
    //{
    //    model.Update(vulkanEngine, camera, light.light);
    //}
    //Skybox.UpdateUniformBuffer(vulkanEngine, camera);
}
