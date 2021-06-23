#include "Engine.h"
#include "MegaMan.h"
#include "Skybox.h"
#include "TerrainMesh.h"
#include "BillboardMesh.h"
#include "Mario.h"
#include "SparkManStage.h"

Engine::Engine()
{
}

Engine::Engine(unsigned int width, unsigned int height, const char* WindowName)
{
    window = std::make_shared<VulkanWindow>(VulkanWindow(1280, 720, "VulkanEngine"));
    engine = VulkanEngine(window);
    assetManager = std::make_shared<AssetManager>(AssetManager(engine, window));

    assetManager->meshManager.MeshList.emplace_back(std::make_shared<MegaMan>(MegaMan(engine, assetManager, glm::vec3(-2.0f, 0.0f, 0.0f))));
    assetManager->meshManager.MeshList.emplace_back(std::make_shared<SparkManStage>(SparkManStage(engine, assetManager, glm::vec3(0.0f, 0.0f, 0.0f))));
 /*   assetManager->meshManager.MeshList.emplace_back(std::make_shared<MegaMan>(MegaMan(engine, assetManager, glm::vec3(1.0f, 0.0f, 0.0f))));
    assetManager->meshManager.MeshList.emplace_back(std::make_shared<MegaMan>(MegaMan(engine, assetManager, glm::vec3(2.0f, 0.0f, 0.0f))));
  */ 
  // assetManager->meshManager.MeshList.emplace_back(std::make_shared<MegaMan>(MegaMan(engine, assetManager, glm::vec3(3.0f, 0.0f, 0.0f))));
    //assetManager->meshManager.MeshList.emplace_back(std::make_shared<Mario>(Mario(engine, assetManager, glm::vec3(4.0f, 0.0f, 0.0f))));

    assetManager->AddModel();
    assetManager->modelManager.ModelList.back()->AddMesh(engine, assetManager->meshManager.MeshList[0]);
    assetManager->modelManager.ModelList.back()->AddMesh(engine, assetManager->meshManager.MeshList[1]);
    ////assetManager->modelManager.ModelList.back()->AddMesh(engine, assetManager->meshManager.MeshList[2]);
    ////assetManager->modelManager.ModelList.back()->AddMesh(engine, assetManager->meshManager.MeshList[3]);
    //std::shared_ptr<Material> material2 = std::make_shared<Material>(engine, assetManager->textureManager);
    //material2->materialTexture.DiffuseMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/FireBillboard.png", VK_FORMAT_R8G8B8A8_SRGB);
    //material2->materialTexture.AlphaMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/FireBillboardAlpha.png", VK_FORMAT_R8G8B8A8_UNORM);
    //std::shared_ptr<Material> MaterialID2 = assetManager->materialManager.LoadMaterial(engine, "FireMaterial", material2);
    //assetManager->meshManager.MeshList.emplace_back(std::make_shared<BillboardMesh>(BillboardMesh(engine, glm::vec2(3.0f), glm::vec2(1.0f), glm::vec3(2.0f), MaterialID2)));

    //assetManager->AddModel(engine, "../Models/Crate.dae");
    //std::shared_ptr<Material> material = std::make_shared<Material>(engine, assetManager->textureManager);
    //material->materialTexture.DiffuseMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/MetalMan_diffuseOriginal.bmp", VK_FORMAT_R8G8B8A8_SRGB);/*
    //material->materialTexture.AlbedoMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/pbr/rusted_iron/albedo.png", VK_FORMAT_R8G8B8A8_SRGB);*/
    //material->materialTexture.NormalMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/MetalMan_normal.bmp", VK_FORMAT_R8G8B8A8_UNORM);
    ////material->materialTexture.RoughnessMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/pbr/rusted_iron/roughness.png", VK_FORMAT_R8G8B8A8_UNORM);
    ////material->materialTexture.AOMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/pbr/rusted_iron/ao.png", VK_FORMAT_R8G8B8A8_UNORM);
    ////material->materialTexture.MatallicMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/pbr/rusted_iron/metallic.png", VK_FORMAT_R8G8B8A8_UNORM);
    //material->materialTexture.DepthMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/MetalMan_height.bmp", VK_FORMAT_R8G8B8A8_UNORM);
    //std::shared_ptr<Material> MaterialID = assetManager->materialManager.LoadMaterial(engine, "MarioMaterial", material);
    //assetManager->modelManager.ModelList[1]->MeshList[0]->MeshMaterial = MaterialID;

    assetManager->textureManager.Load3DTexture(engine, "C:/Users/dotha/Desktop/detailed_surfaces/media/sculptureSphere.dds", VK_FORMAT_R8_UNORM);
  // assetManager->AddModel(engine, "../Models/viking_room.obj");
    //std::shared_ptr<Material> material = std::make_shared<Material>(engine, assetManager->textureManager);
    //material->materialTexture.DiffuseMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/container2.png", VK_FORMAT_R8G8B8A8_SRGB);
    //material->materialTexture.AlbedoMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/pbr/plastic/albedo.png", VK_FORMAT_R8G8B8A8_SRGB);
    //material->materialTexture.NormalMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/pbr/plastic/normal.png", VK_FORMAT_R8G8B8A8_UNORM);
    //material->materialTexture.RoughnessMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/pbr/plastic/roughness.png", VK_FORMAT_R8G8B8A8_UNORM);
    //material->materialTexture.AOMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/pbr/plastic/ao.png", VK_FORMAT_R8G8B8A8_UNORM);
    //material->materialTexture.MatallicMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/pbr/plastic/metallic.png", VK_FORMAT_R8G8B8A8_UNORM);
    //material->materialTexture.DepthMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/toy_box_disp.png", VK_FORMAT_R8G8B8A8_UNORM);
    //uint32_t MaterialID = assetManager->materialManager.LoadMaterial(engine, "MarioMaterial", material);
    //assetManager->modelManager.ModelList[1]->MeshList[0]->MaterialID = MaterialID;
    assetManager->meshManager.MeshList.emplace_back(std::make_shared<Skybox>(Skybox(engine, assetManager)));
    //assetManager->AddModel(engine, "../Models/RayReflectionTest.obj");
   // assetManager->AddModel(engine, "../Models/TestAnimModel/model.dae");

  /*   assetManager.AddModel(engine, "../Models/EnemyBeast.fbx");
     assetManager.AddModel(engine, "../Models/PlayerMarine.fbx");*/

    std::string CubeMapFiles[6];
    CubeMapFiles[0] = "../texture/skybox/right.jpg";
    CubeMapFiles[1] = "../texture/skybox/left.jpg";
    CubeMapFiles[2] = "../texture/skybox/top.jpg";
    CubeMapFiles[3] = "../texture/skybox/bottom.jpg";
    CubeMapFiles[4] = "../texture/skybox/back.jpg";
    CubeMapFiles[5] = "../texture/skybox/front.jpg";

    std::string CubeMapFiles2[6];
    CubeMapFiles2[0] = "../texture/skybox/CosmicCoolCloudRight.png";
    CubeMapFiles2[1] = "../texture/skybox/CosmicCoolCloudLeft.png";
    CubeMapFiles2[2] = "../texture/skybox/CosmicCoolCloudFront.png";
    CubeMapFiles2[3] = "../texture/skybox/CosmicCoolCloudBottom.png";
    CubeMapFiles2[4] = "../texture/skybox/CosmicCoolCloudBack.png";
    CubeMapFiles2[5] = "../texture/skybox/CosmicCoolCloudTop.png";

    assetManager->textureManager.LoadCubeMap(engine, CubeMapFiles, VK_FORMAT_R8G8B8A8_UNORM);



    //assetManager->meshManager.MeshList.emplace_back(std::make_shared<TerrainMesh>(TerrainMesh(engine, "../texture/perlin_noise.png")));
    //std::shared_ptr<Material> material1 = std::make_shared<Material>(engine, assetManager->textureManager);
    //material1->materialTexture.DiffuseMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/forrest_ground_01_diff_4k.jpg", VK_FORMAT_R8G8B8A8_SRGB);
    //material1->materialTexture.AlbedoMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/forrest_ground_01_diff_4k.jpg", VK_FORMAT_R8G8B8A8_SRGB);
    //material1->materialTexture.RoughnessMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/forrest_ground_01_rough_4k.jpg", VK_FORMAT_R8G8B8A8_UNORM);
    //material1->materialTexture.AOMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/forrest_ground_01_ao_4k.jpg", VK_FORMAT_R8G8B8A8_UNORM);
    //material1->materialTexture.NormalMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/forrest_ground_01_nor_4k.jpg", VK_FORMAT_R8G8B8A8_UNORM);
    //material1->materialTexture.DepthMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/forrest_ground_01_disp_4k.jpg", VK_FORMAT_R8G8B8A8_UNORM);
    //material1->materialTexture.MatallicMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/forrest_ground_01_disp_4k.jpg", VK_FORMAT_R8G8B8A8_UNORM);
    //std::shared_ptr<Material> MaterialID1 = assetManager->materialManager.LoadMaterial(engine, "MarioMaterial", material1);
    //assetManager->meshManager.MeshList.back()->MeshMaterial = MaterialID1;
    //assetManager->modelManager.ModelList[0]->MeshList[0]->MeshMaterial = MaterialID;



 //   assetManager->AddModel(engine, "../Models/vulkanscene_shadow.obj");
 ///  assetManager->AddModel(engine, "../Models/Sponza/Sponza.obj");
    assetManager->SceneData->UniformDataInfo.dlight.direction = glm::vec4(0.0f);
    assetManager->SceneData->UniformDataInfo.dlight.ambient = glm::vec4(0.2f);
    assetManager->SceneData->UniformDataInfo.dlight.diffuse = glm::vec4(0.5f);
    assetManager->SceneData->UniformDataInfo.dlight.specular = glm::vec4(1.0f);

    assetManager->SceneData->UniformDataInfo.plight[0].position = glm::vec4(0.5f, 1.0f, 0.3f, 1.0f);
    assetManager->SceneData->UniformDataInfo.plight[0].ambient = glm::vec4(0.2f);
    assetManager->SceneData->UniformDataInfo.plight[0].diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 0.0f);
    assetManager->SceneData->UniformDataInfo.plight[0].specular = glm::vec4(1.0f);

    assetManager->SceneData->UniformDataInfo.plight[1].position = glm::vec4(0.5f, 1.0f, 0.3f, 1.0f);
    assetManager->SceneData->UniformDataInfo.plight[1].ambient = glm::vec4(0.2f);
    assetManager->SceneData->UniformDataInfo.plight[1].diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 0.0f);
    assetManager->SceneData->UniformDataInfo.plight[1].specular = glm::vec4(1.0f);

    assetManager->SceneData->UniformDataInfo.plight[2].position = glm::vec4(0.5f, 1.0f, 0.3f, 1.0f);
    assetManager->SceneData->UniformDataInfo.plight[2].ambient = glm::vec4(0.2f);
    assetManager->SceneData->UniformDataInfo.plight[2].diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 0.0f);
    assetManager->SceneData->UniformDataInfo.plight[2].specular = glm::vec4(1.0f);

    assetManager->SceneData->UniformDataInfo.plight[3].position = glm::vec4(0.5f, 1.0f, 0.3f, 1.0f);
    assetManager->SceneData->UniformDataInfo.plight[3].ambient = glm::vec4(0.2f);
    assetManager->SceneData->UniformDataInfo.plight[3].diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 0.0f);
    assetManager->SceneData->UniformDataInfo.plight[3].specular = glm::vec4(1.0f);

    assetManager->SceneData->UniformDataInfo.plight[4].position = glm::vec4(0.5f, 1.0f, 0.3f, 1.0f);
    assetManager->SceneData->UniformDataInfo.plight[4].ambient = glm::vec4(0.2f);
    assetManager->SceneData->UniformDataInfo.plight[4].diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 0.0f);
    assetManager->SceneData->UniformDataInfo.plight[4].specular = glm::vec4(1.0f);

    assetManager->SceneData->UniformDataInfo.sLight.ambient = glm::vec4(0.0f);
    assetManager->SceneData->UniformDataInfo.sLight.diffuse = glm::vec4(1.0f);
    assetManager->SceneData->UniformDataInfo.sLight.specular = glm::vec4(1.0f);
    renderer = Renderer(engine, window, assetManager);
}

Engine::~Engine()
{
    assetManager->Delete(engine);
    renderer.Destroy(engine);
    engine.Destroy();
    window->Destroy();
}

void Engine::MainLoop()
{
    while (!glfwWindowShouldClose(window->GetWindowPtr())) 
    {
        glfwPollEvents();

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            renderer.GUIUpdate(engine);
        }
        ImGui::Render();

        renderer.Draw(engine, window);
    }

    vkDeviceWaitIdle(engine.Device);
}