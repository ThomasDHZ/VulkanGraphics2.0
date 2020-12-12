#include "Game.h"
#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <stb_image.h>
#include "MegaMan.h"
#include "EnergyTank.h"
#define STB_IMAGE_IMPLEMENTATION

Game::Game()
{

    //DDS_HEADER header;
    //std::ifstream file2("C:/Users/dotha/source/repos/VulkanGraphics/VulkanGraphics/texture/skybox/front.dds", std::ios::binary);
    //file2.read(reinterpret_cast<char*>(&header), sizeof(header));

    window = VulkanWindow(800, 600, "Vulkan Engine");
    vulkanEngine = VulkanEngine(window.GetWindowPtr());
    textureManager = std::make_shared<TextureManager>(vulkanEngine);
    renderManager = RenderManager(vulkanEngine, textureManager, window.GetWindowPtr());

    light = LightManager(vulkanEngine, textureManager, renderManager.mainRenderPass.debugLightRenderingPipeline->ShaderPipelineDescriptorLayout, RenderDrawFlags::RenderLightDebug, glm::vec3(0.0f));

    MeshTextures meshTextures;
    meshTextures.CubeMap[0] = "C:/Users/dotha/source/repos/VulkanGraphics/texture/skybox/left.jpg";
    meshTextures.CubeMap[1] = "C:/Users/dotha/source/repos/VulkanGraphics/texture/skybox/right.jpg";
    meshTextures.CubeMap[2] = "C:/Users/dotha/source/repos/VulkanGraphics/texture/skybox/top.jpg";
    meshTextures.CubeMap[3] = "C:/Users/dotha/source/repos/VulkanGraphics/texture/skybox/bottom.jpg";
    meshTextures.CubeMap[4] = "C:/Users/dotha/source/repos/VulkanGraphics/texture/skybox/back.jpg";
    meshTextures.CubeMap[5] = "C:/Users/dotha/source/repos/VulkanGraphics/texture/skybox/front.jpg";

    camera = std::make_shared<PerspectiveCamera>(PerspectiveCamera(glm::vec2(vulkanEngine.SwapChain.GetSwapChainResolution().width / (float)vulkanEngine.SwapChain.GetSwapChainResolution().height), glm::vec3(0.0f)));
    

    SpriteList.emplace_back(std::make_shared<EnergyTank>(EnergyTank(vulkanEngine, textureManager, glm::vec2(0.0f), renderManager.mainRenderPass.forwardRenderering2DPipeline->ShaderPipelineDescriptorLayout)));

    Skybox = SkyBoxMesh(vulkanEngine, textureManager, renderManager.mainRenderPass.skyBoxPipeline->ShaderPipelineDescriptorLayout, meshTextures);
    renderManager.CMDBuffer(vulkanEngine, camera, ModelList, Skybox, light, SpriteList);
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
    while (!glfwWindowShouldClose(window.GetWindowPtr()))
    {
        glfwPollEvents();

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
      /*      ImGui::CheckboxFlags("Normally", &ModelList[0].MeshList[0]->RenderFlags, RenderDrawFlags::RenderNormally);
            ImGui::CheckboxFlags("WireFrame", &ModelList[0].MeshList[0]->RenderFlags, RenderDrawFlags::RenderWireFrame);
            ImGui::CheckboxFlags("Shadow", &ModelList[0].MeshList[0]->RenderFlags, RenderDrawFlags::RenderShadow);*/

            /*          ImGui::CheckboxFlags("Normally2", &ModelList[1].MeshList[0]->RenderFlags, RenderDrawFlags::RenderNormally);
                      ImGui::CheckboxFlags("WireFrame2", &ModelList[1].MeshList[0]->RenderFlags, RenderDrawFlags::RenderWireFrame);
                      ImGui::CheckboxFlags("Shadow2", &ModelList[1].MeshList[0]->RenderFlags, RenderDrawFlags::RenderShadow);

                      ImGui::CheckboxFlags("Normally3", &ModelList[2].MeshList[0]->RenderFlags, RenderDrawFlags::RenderNormally);
                      ImGui::CheckboxFlags("WireFrame3", &ModelList[2].MeshList[0]->RenderFlags, RenderDrawFlags::RenderWireFrame);
                      ImGui::CheckboxFlags("Shadow3", &ModelList[2].MeshList[0]->RenderFlags, RenderDrawFlags::RenderShadow);

                      ImGui::CheckboxFlags("Normally4", &ModelList[3].MeshList[0]->RenderFlags, RenderDrawFlags::RenderNormally);
                      ImGui::CheckboxFlags("WireFrame4", &ModelList[3].MeshList[0]->RenderFlags, RenderDrawFlags::RenderWireFrame);
                      ImGui::CheckboxFlags("Shadow4", &ModelList[3].MeshList[0]->RenderFlags, RenderDrawFlags::RenderShadow);*/

            //ImGui::Image(renderManager.gBufferRenderPass.SSAOTexture->ImGuiDescriptorSet, ImVec2(80.0f, 80.0f));
            ////ImGui::Image(renderManager.gBufferRenderPass.SSAOBlurTexture->ImGuiDescriptorSet, ImVec2(80.0f, 80.0f));
            //ImGui::Image(renderManager.gBufferRenderPass.GPositionTexture->ImGuiDescriptorSet, ImVec2(80.0f, 80.0f));
            //ImGui::Image(renderManager.gBufferRenderPass.GNormalTexture->ImGuiDescriptorSet, ImVec2(80.0f, 80.0f));
            //ImGui::Image(renderManager.gBufferRenderPass.GAlbedoTexture->ImGuiDescriptorSet, ImVec2(80.0f, 80.0f));
            ImGui::Image(renderManager.textureRenderPass.ColorTexture->ImGuiDescriptorSet, ImVec2(80.0f, 80.0f));
            ImGui::Image(renderManager.bloomRenderPass.GetIMGuiImagePass1(), ImVec2(300.0f, 300.0f));
            ImGui::Image(renderManager.bloomRenderPass.GetIMGuiImagePass2(), ImVec2(300.0f, 300.0f));
            //ImGui::Image(renderManager.shadowRenderPass.DebugColorTexture->ImGuiDescriptorSet, ImVec2(80.0f, 80.0f));

            ImGui::SliderFloat3("dLight", &light.light.dLight.direction.x, -10.0f, 10.0f);
            ImGui::SliderFloat3("dambient", &light.light.dLight.ambient.x, 0.0f, 1.0f);
            ImGui::SliderFloat3("ddiffuse", &light.light.dLight.diffuse.x, 0.0f, 1.0f);
            ImGui::SliderFloat3("dspecular", &light.light.dLight.specular.x, 0.0f, 1.0f);

            ImGui::SliderInt("Using1", &light.light.pLight[0].InUseFlag, 0.0f, 1.0f);
            ImGui::SliderFloat3("pLight", &light.light.pLight[0].position.x, -10.0f, 10.0f);
            ImGui::SliderFloat3("pambient", &light.light.pLight[0].ambient.x, 0.0f, 1.0f);
            ImGui::SliderFloat3("pdiffuse", &light.light.pLight[0].diffuse.x, 0.0f, 1.0f);
            ImGui::SliderFloat3("pspecular", &light.light.pLight[0].specular.x, 0.0f, 1.0f);

            ImGui::SliderInt("Using2", &light.light.pLight[1].InUseFlag, 0.0f, 1.0f);
            ImGui::SliderFloat3("pLight2", &light.light.pLight[1].position.x, -10.0f, 10.0f);
            ImGui::SliderFloat3("pambient2", &light.light.pLight[1].ambient.x, 0.0f, 1.0f);
            ImGui::SliderFloat3("pdiffuse2", &light.light.pLight[1].diffuse.x, 0.0f, 1.0f);
            ImGui::SliderFloat3("pspecular2", &light.light.pLight[1].specular.x, 0.0f, 1.0f);

            ImGui::SliderInt("Using3", &light.light.pLight[2].InUseFlag, 0.0f, 1.0f);
            ImGui::SliderFloat3("pLight3", &light.light.pLight[2].position.x, -10.0f, 10.0f);
            ImGui::SliderFloat3("pambient3", &light.light.pLight[2].ambient.x, 0.0f, 1.0f);
            ImGui::SliderFloat3("pdiffuse3", &light.light.pLight[2].diffuse.x, 0.0f, 1.0f);
            ImGui::SliderFloat3("pspecular3", &light.light.pLight[2].specular.x, 0.0f, 1.0f);

            ImGui::SliderInt("Using4", &light.light.pLight[3].InUseFlag, 0.0f, 1.0f);
            ImGui::SliderFloat3("pLight4", &light.light.pLight[3].position.x, -10.0f, 10.0f);
            ImGui::SliderFloat3("pambient4", &light.light.pLight[3].ambient.x, 0.0f, 1.0f);
            ImGui::SliderFloat3("pdiffuse4", &light.light.pLight[3].diffuse.x, 0.0f, 1.0f);
            ImGui::SliderFloat3("pspecular4", &light.light.pLight[3].specular.x, 0.0f, 1.0f);

            renderManager.sceneRenderPass.FrameDebug();
            textureManager->UpdateIMGUIVRAM();
        }
        ImGui::Render();

        //renderManager.UpdateCommandBuffer(vulkanEngine, ModelList, Skybox);
        UpdateUniformBuffer(vulkanEngine.DrawFrame);
        renderManager.RendererUpdate(vulkanEngine, camera);
        renderManager.Draw(vulkanEngine, window.GetWindowPtr(), textureManager, camera, ModelList, Skybox, light, SpriteList);
        mouse.Update(window.GetWindowPtr(), camera);
        keyboard.Update(window.GetWindowPtr(), camera);
    }

    vkDeviceWaitIdle(vulkanEngine.Device);
}

void Game::UpdateUniformBuffer(uint32_t currentImage)
{
    light.Update(vulkanEngine, camera);
    camera->Update(vulkanEngine);

    for (auto& model : ModelList)
    {
        model.Update(vulkanEngine, camera, light.light);
    }
    for (auto& sprite : SpriteList)
    {
        if (auto MM = dynamic_cast<MegaMan*>(sprite.get()))
        {
           // MM->Update(window.GetWindowPtr(), vulkanEngine, camera, 1.1f, light.light, SpriteList, textureManager);
        }
        else
        {
            sprite->Update(vulkanEngine, 1.1, camera, light.light);
        }

   /*     sprite->Gravity(level.LevelColliderList);
        sprite->Update(renderer, camera, light.light);
        sprite->Collision(renderer, SpriteList);*/
    }
    Skybox.UpdateUniformBuffer(vulkanEngine, camera);
}
