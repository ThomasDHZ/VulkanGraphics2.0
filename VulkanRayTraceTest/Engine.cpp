#include "engine.h"
#include "MegaMan.h"
#include "Skybox.h"
#include "TerrainMesh.h"
#include "BillboardMesh.h"
#include "Mario.h"
#include "SparkManStage.h"
#include "LavaTest.h"
#include "TextMesh.h"
#include <ft2build.h>
#include "GUIPanel.h"
#include "GameObject.h"
#include FT_FREETYPE_H

Engine::Engine()
{
}

Engine::Engine(unsigned int width, unsigned int height, const char* WindowName)
{
    WindowPtr::SetUpPtr(width, height, WindowName);
    EnginePtr::SetUpPtr(WindowPtr::GetWindowPtr());
    CameraManagerPtr::SetUpPtr(EnginePtr::GetEnginePtr());
    InputManagerPtr::SetUpPtr(WindowPtr::GetWindowPtr(), CameraManagerPtr::GetCameraManagerPtr());
    TextureManagerPtr::SetUpPtr(EnginePtr::GetEnginePtr());
    MaterialManagerPtr::SetUpPtr(EnginePtr::GetEnginePtr());
    MeshManagerPtr::SetUpPtr(EnginePtr::GetEnginePtr(), InputManagerPtr::GetInputManagerPtr(), MaterialManagerPtr::GetMaterialManagerPtr());
    LightManagerPtr::SetUpPtr(EnginePtr::GetEnginePtr(), CameraManagerPtr::GetCameraManagerPtr());
    GuiManagerPtr::SetUpPtr(EnginePtr::GetEnginePtr());
    ObjManagerPtr::SetUpPtr(EnginePtr::GetEnginePtr(), MaterialManagerPtr::GetMaterialManagerPtr(), InputManagerPtr::GetInputManagerPtr(), TextureManagerPtr::GetTextureManagerPtr());
    AssetManagerPtr::SetUpPtr(EnginePtr::GetEnginePtr());

    std::shared_ptr<GameObject> gameObject = std::make_shared<GameObject>(GameObject(EnginePtr::GetEnginePtr()));
    gameObject->AddChildModel(AssetManagerPtr::GetAssetPtr()->meshManager, std::make_shared<SparkManStage>(SparkManStage(EnginePtr::GetEnginePtr(), AssetManagerPtr::GetAssetPtr(), glm::vec3(0.0f, 0.0f, 0.0f))));
    AssetManagerPtr::GetAssetPtr()->ObjManager->ObjectList.emplace_back(gameObject);

    //std::shared_ptr<GameObject> gameObject2 = std::make_shared<GameObject>(GameObject(EnginePtr::GetEnginePtr()));
    //gameObject2->AddChildModel(AssetManagerPtr::GetAssetPtr()->meshManager, std::make_shared<Model>(Model(EnginePtr::GetEnginePtr(), AssetManagerPtr::GetAssetPtr()->materialManager, AssetManagerPtr::GetAssetPtr()->textureManager, "../Models/TestAnimModel/model.dae")));
    //AssetManagerPtr::GetAssetPtr()->ObjManager->ObjectList.emplace_back(gameObject2);

    std::shared_ptr<GameObject> gameObject3 = std::make_shared<GameObject>(GameObject(EnginePtr::GetEnginePtr()));
    gameObject3->AddChildMesh(AssetManagerPtr::GetAssetPtr()->meshManager, std::make_shared<Skybox>(Skybox(EnginePtr::GetEnginePtr(), AssetManagerPtr::GetAssetPtr())));
    AssetManagerPtr::GetAssetPtr()->ObjManager->ObjectList.emplace_back(gameObject3);


    MaterialTexture material;
    material.DiffuseMap = LoadTexture2D("../texture/forrest_ground_01_diff_4k.jpg", VK_FORMAT_R8G8B8A8_SRGB);
    material.AlbedoMap = LoadTexture2D("../texture/forrest_ground_01_diff_4k.jpg", VK_FORMAT_R8G8B8A8_SRGB);
    material.RoughnessMap = LoadTexture2D("../texture/forrest_ground_01_rough_4k.jpg", VK_FORMAT_R8G8B8A8_UNORM);
    material.AOMap = LoadTexture2D("../texture/forrest_ground_01_ao_4k.jpg", VK_FORMAT_R8G8B8A8_UNORM);
    material.NormalMap = LoadTexture2D("../texture/forrest_ground_01_nor_4k.jpg", VK_FORMAT_R8G8B8A8_UNORM);
    material.DepthMap = LoadTexture2D("../texture/forrest_ground_01_disp_4k.jpg", VK_FORMAT_R8G8B8A8_UNORM);
    material.MatallicMap = LoadTexture2D("../texture/forrest_ground_01_disp_4k.jpg", VK_FORMAT_R8G8B8A8_UNORM);

    std::shared_ptr<Material> materialPtr = LoadMaterial("TerrianMaterial", material);

    //std::shared_ptr<GameObject> gameObject4 = std::make_shared<GameObject>(GameObject(EnginePtr::GetEnginePtr()));
    //gameObject4->AddChildMesh(AssetManagerPtr::GetAssetPtr()->meshManager, std::make_shared<TerrainMesh>(TerrainMesh(EnginePtr::GetEnginePtr(), "../texture/perlin_noise.png", materialPtr)));
    //AssetManagerPtr::GetAssetPtr()->ObjManager->ObjectList.emplace_back(gameObject4);

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

    AssetManagerPtr::GetAssetPtr()->textureManager->Load3DTexture("C:/Users/dotha/Desktop/detailed_surfaces/media/sculptureSphere.dds", VK_FORMAT_R8_UNORM);
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
    AssetManagerPtr::GetAssetPtr()->meshManager->AddMesh(std::make_shared<Skybox>(Skybox(EnginePtr::GetEnginePtr(), AssetManagerPtr::GetAssetPtr())));
    //LoadModel("../Models/RayReflectionTest.obj");
    //LoadModel("../Models/TestAnimModel/model.dae");
    //LoadModel("../Models/vulkanscene_shadow.obj");
  //  LoadModel("../Models/Sponza/Sponza.obj");
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

    LoadCubeMap(CubeMapFiles);



    GuiRectangle rectangle;
    rectangle.UpperLeftVertex = glm::vec2(-0.5f, -0.5f);
    rectangle.UpperRightVertex = glm::vec2(0.5f, -0.5f);
    rectangle.LowerRightVertex = glm::vec2(0.5f, 0.5f);
    rectangle.LowerLeftVertex = glm::vec2(-0.5f, 0.5f);
    AssetManagerPtr::GetAssetPtr()->ObjManager->ObjectList.emplace_back(std::make_shared<GUIPanel>(GUIPanel(EnginePtr::GetEnginePtr(), materialPtr, rectangle, glm::vec2(0.0f))));
    AssetManagerPtr::GetAssetPtr()->ObjManager->ObjectList.emplace_back(std::make_shared<TextMesh>(TextMesh(EnginePtr::GetEnginePtr(), AssetManagerPtr::GetAssetPtr()->materialManager, AssetManagerPtr::GetAssetPtr()->textureManager, AssetManagerPtr::GetAssetPtr()->guiManager->FontList[0], "SparkMan", glm::vec2(0.0f))));

    AssetManagerPtr::GetAssetPtr()->SceneData->UniformDataInfo.dlight.direction = glm::vec4(0.0f);
    AssetManagerPtr::GetAssetPtr()->SceneData->UniformDataInfo.dlight.ambient = glm::vec4(0.2f);
    AssetManagerPtr::GetAssetPtr()->SceneData->UniformDataInfo.dlight.diffuse = glm::vec4(0.5f);
    AssetManagerPtr::GetAssetPtr()->SceneData->UniformDataInfo.dlight.specular = glm::vec4(1.0f);

    AssetManagerPtr::GetAssetPtr()->SceneData->UniformDataInfo.plight[0].position = glm::vec4(0.5f, 1.0f, 0.3f, 1.0f);
    AssetManagerPtr::GetAssetPtr()->SceneData->UniformDataInfo.plight[0].ambient = glm::vec4(0.2f);
    AssetManagerPtr::GetAssetPtr()->SceneData->UniformDataInfo.plight[0].diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 0.0f);
    AssetManagerPtr::GetAssetPtr()->SceneData->UniformDataInfo.plight[0].specular = glm::vec4(1.0f);

    AssetManagerPtr::GetAssetPtr()->SceneData->UniformDataInfo.plight[1].position = glm::vec4(0.5f, 1.0f, 0.3f, 1.0f);
    AssetManagerPtr::GetAssetPtr()->SceneData->UniformDataInfo.plight[1].ambient = glm::vec4(0.2f);
    AssetManagerPtr::GetAssetPtr()->SceneData->UniformDataInfo.plight[1].diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 0.0f);
    AssetManagerPtr::GetAssetPtr()->SceneData->UniformDataInfo.plight[1].specular = glm::vec4(1.0f);

    AssetManagerPtr::GetAssetPtr()->SceneData->UniformDataInfo.plight[2].position = glm::vec4(0.5f, 1.0f, 0.3f, 1.0f);
    AssetManagerPtr::GetAssetPtr()->SceneData->UniformDataInfo.plight[2].ambient = glm::vec4(0.2f);
    AssetManagerPtr::GetAssetPtr()->SceneData->UniformDataInfo.plight[2].diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 0.0f);
    AssetManagerPtr::GetAssetPtr()->SceneData->UniformDataInfo.plight[2].specular = glm::vec4(1.0f);

    AssetManagerPtr::GetAssetPtr()->SceneData->UniformDataInfo.plight[3].position = glm::vec4(0.5f, 1.0f, 0.3f, 1.0f);
    AssetManagerPtr::GetAssetPtr()->SceneData->UniformDataInfo.plight[3].ambient = glm::vec4(0.2f);
    AssetManagerPtr::GetAssetPtr()->SceneData->UniformDataInfo.plight[3].diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 0.0f);
    AssetManagerPtr::GetAssetPtr()->SceneData->UniformDataInfo.plight[3].specular = glm::vec4(1.0f);

    AssetManagerPtr::GetAssetPtr()->SceneData->UniformDataInfo.plight[4].position = glm::vec4(0.5f, 1.0f, 0.3f, 1.0f);
    AssetManagerPtr::GetAssetPtr()->SceneData->UniformDataInfo.plight[4].ambient = glm::vec4(0.2f);
    AssetManagerPtr::GetAssetPtr()->SceneData->UniformDataInfo.plight[4].diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 0.0f);
    AssetManagerPtr::GetAssetPtr()->SceneData->UniformDataInfo.plight[4].specular = glm::vec4(1.0f);

    AssetManagerPtr::GetAssetPtr()->SceneData->UniformDataInfo.sLight.ambient = glm::vec4(0.0f);
    AssetManagerPtr::GetAssetPtr()->SceneData->UniformDataInfo.sLight.diffuse = glm::vec4(1.0f);
    AssetManagerPtr::GetAssetPtr()->SceneData->UniformDataInfo.sLight.specular = glm::vec4(1.0f);

    renderer = Renderer(EnginePtr::GetEnginePtr(), WindowPtr::GetWindowPtr(), AssetManagerPtr::GetAssetPtr());
}

Engine::~Engine()
{
    AssetManagerPtr::GetAssetPtr()->Delete();
    renderer.Destroy(EnginePtr::GetEnginePtr());
    EnginePtr::GetEnginePtr()->Destroy();
    WindowPtr::GetWindowPtr()->Destroy();
}

void Engine::MainLoop()
{
    while (!glfwWindowShouldClose(WindowPtr::GetWindowPtr()->GetWindowPtr()))
    {
        glfwPollEvents();

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            renderer.GUIUpdate(EnginePtr::GetEnginePtr());
        }
        ImGui::Render();

        renderer.Draw(EnginePtr::GetEnginePtr(), WindowPtr::GetWindowPtr());
    }

    vkDeviceWaitIdle(EnginePtr::GetEnginePtr()->Device);
}

std::shared_ptr<Texture> Engine::LoadTexture2D(const std::string& FilePath, VkFormat format)
{
    return AssetManagerPtr::GetAssetPtr()->textureManager->LoadTexture2D(FilePath, format);
}

std::shared_ptr<Material> Engine::LoadMaterial(const std::string& MaterialName, MaterialTexture& material)
{
    return AssetManagerPtr::GetAssetPtr()->materialManager->LoadMaterial(MaterialName, material);
}

std::shared_ptr<Material> Engine::LoadMaterial(const std::string& MaterialName, std::shared_ptr<Material> material)
{
    return AssetManagerPtr::GetAssetPtr()->materialManager->LoadMaterial(MaterialName, material);
}

void Engine::LoadCubeMap(std::string CubeMapFiles[6])
{
    AssetManagerPtr::GetAssetPtr()->textureManager->LoadCubeMap(CubeMapFiles, VK_FORMAT_R8G8B8A8_UNORM);
}

void Engine::LoadTerrain(const std::string& HeightMapPath, std::shared_ptr<Material> material)
{
    AssetManagerPtr::GetAssetPtr()->meshManager->AddMesh(std::make_shared<TerrainMesh>(TerrainMesh(EnginePtr::GetEnginePtr(), HeightMapPath, material)));
}
