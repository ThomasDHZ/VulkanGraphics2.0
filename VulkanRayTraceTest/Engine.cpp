#include "Engine.h"
#include "MegaMan.h"

Engine::Engine()
{
}

Engine::Engine(unsigned int width, unsigned int height, const char* WindowName)
{
    window = VulkanWindow(1280, 720, "VulkanEngine");
    engine = VulkanEngine(window.GetWindowPtr());
    assetManager = std::make_shared<AssetManager>(AssetManager(engine));

    assetManager->meshManager.MeshList.emplace_back(std::make_shared<MegaMan>(MegaMan(engine, assetManager, glm::vec3(1.0f, 0.0f, 0.0f))));
    //assetManager.meshManager.MeshList.emplace_back(std::make_shared<MegaMan>(MegaMan(engine, assetManager, glm::vec3(1.0f, 0.0f, 0.0f))));
    //assetManager.meshManager.MeshList.emplace_back(std::make_shared<MegaMan>(MegaMan(engine, assetManager, glm::vec3(2.0f, 0.0f, 0.0f))));
    //assetManager.meshManager.MeshList.emplace_back(std::make_shared<MegaMan>(MegaMan(engine, assetManager, glm::vec3(3.0f, 0.0f, 0.0f))));
    //assetManager.meshManager.MeshList.emplace_back(std::make_shared<Mario>(Mario(engine, assetManager, glm::vec3(4.0f, 0.0f, 0.0f))));

    assetManager->AddModel();
    assetManager->modelManager.ModelList.back()->AddMesh(engine, assetManager->meshManager.MeshList[0]);
    //assetManager.modelManager.ModelList.back()->AddMesh(engine, assetManager.meshManager.MeshList[1]);
    //assetManager.modelManager.ModelList.back()->AddMesh(engine, assetManager.meshManager.MeshList[2]);
    //assetManager.modelManager.ModelList.back()->AddMesh(engine, assetManager.meshManager.MeshList[3]);

    //assetManager.AddModel(engine, "../Models/Crate.dae");
    //std::shared_ptr<Material> material = std::make_shared<Material>(engine, assetManager.textureManager);
    //material->materialTexture.DiffuseMap = assetManager.textureManager.LoadTexture2D(engine, "../texture/toy_box_diffuse.png", VK_FORMAT_R8G8B8A8_UNORM);
    //material->materialTexture.NormalMap = assetManager.textureManager.LoadTexture2D(engine, "../texture/toy_box_normal.png", VK_FORMAT_R8G8B8A8_UNORM);
    //material->materialTexture.DepthMap = assetManager.textureManager.LoadTexture2D(engine, "../texture/toy_box_disp.png", VK_FORMAT_R8G8B8A8_UNORM);
    //uint32_t MaterialID = assetManager.materialManager.LoadMaterial(engine, "MarioMaterial", material);
    //assetManager.modelManager.ModelList[1]->MeshList[0]->MaterialID = MaterialID;

    assetManager->AddModel(engine, "../Models/viking_room.obj");
    std::shared_ptr<Material> material = std::make_shared<Material>(engine, assetManager->textureManager);
    material->materialTexture.DiffuseMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/container2.png", VK_FORMAT_R8G8B8A8_SRGB);
    material->materialTexture.AlbedoMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/pbr/rusted_iron/albedo.png", VK_FORMAT_R8G8B8A8_SRGB);
    material->materialTexture.NormalMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/pbr/rusted_iron/normal.png", VK_FORMAT_R8G8B8A8_UNORM);
    material->materialTexture.RoughnessMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/pbr/rusted_iron/roughness.png", VK_FORMAT_R8G8B8A8_UNORM);
    material->materialTexture.AOMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/pbr/rusted_iron/ao.png", VK_FORMAT_R8G8B8A8_UNORM);
    material->materialTexture.MatallicMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/pbr/rusted_iron/metallic.png", VK_FORMAT_R8G8B8A8_UNORM);
    material->materialTexture.DepthMap = assetManager->textureManager.LoadTexture2D(engine, "../texture/toy_box_disp.png", VK_FORMAT_R8G8B8A8_UNORM);
    uint32_t MaterialID = assetManager->materialManager.LoadMaterial(engine, "MarioMaterial", material);
    assetManager->modelManager.ModelList[1]->MeshList[0]->MaterialID = MaterialID;

    // assetManager.AddModel(engine, "../Models/RayReflectionTest.obj");
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



    //assetManager.meshManager.MeshList.emplace_back(std::make_shared<TerrainMesh>(TerrainMesh(engine, "../texture/perlin_noise.png")));
    //std::shared_ptr<Material> material1 = std::make_shared<Material>(engine, assetManager.textureManager);
    //material1->materialTexture.DiffuseMap = assetManager.textureManager.LoadTexture2D(engine, "../texture/forrest_ground_01_diff_4k.jpg", VK_FORMAT_R8G8B8A8_SRGB);
    //material1->materialTexture.AlbedoMap = assetManager.textureManager.LoadTexture2D(engine, "../texture/forrest_ground_01_diff_4k.jpg", VK_FORMAT_R8G8B8A8_SRGB);
    //material1->materialTexture.RoughnessMap = assetManager.textureManager.LoadTexture2D(engine, "../texture/forrest_ground_01_rough_4k.jpg", VK_FORMAT_R8G8B8A8_UNORM);
    //material1->materialTexture.AOMap = assetManager.textureManager.LoadTexture2D(engine, "../texture/forrest_ground_01_ao_4k.jpg", VK_FORMAT_R8G8B8A8_UNORM);
    //material1->materialTexture.NormalMap = assetManager.textureManager.LoadTexture2D(engine, "../texture/forrest_ground_01_nor_4k.jpg", VK_FORMAT_R8G8B8A8_UNORM);
    //material1->materialTexture.DepthMap = assetManager.textureManager.LoadTexture2D(engine, "../texture/forrest_ground_01_disp_4k.jpg", VK_FORMAT_R8G8B8A8_UNORM);
    //material1->materialTexture.MatallicMap = assetManager.textureManager.LoadTexture2D(engine, "../texture/forrest_ground_01_disp_4k.jpg", VK_FORMAT_R8G8B8A8_UNORM);
    //uint32_t MaterialID1 = assetManager.materialManager.LoadMaterial(engine, "MarioMaterial", material1);
    //assetManager.meshManager.MeshList.back()->MaterialID = MaterialID1;
           //assetManager.modelManager.ModelList[0]->MeshList[0]->MaterialID = MaterialID;



      //assetManager.AddModel(engine, "../Models/cyborg/cyborg.obj");
      // modelRenderManager.AddModel(engine, "../Models/Sponza/Sponza.obj");
    renderer = Renderer(engine, window, assetManager);
}

Engine::~Engine()
{
    assetManager->Delete(engine);
    renderer.Destroy(engine);
    engine.Destroy();
    window.Destroy();
}

void Engine::MainLoop()
{
    while (!glfwWindowShouldClose(window.GetWindowPtr())) 
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