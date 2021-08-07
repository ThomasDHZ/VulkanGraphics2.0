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
    GlobalPtr::SetUpPtrs(width, height, WindowName);

    std::shared_ptr<GameObject> gameObject = std::make_shared<GameObject>(GameObject(GlobalPtr::enginePtr));
    gameObject->AddChildModel(GlobalPtr::assetManagerPtr->meshManager, std::make_shared<SparkManStage>(SparkManStage(GlobalPtr::enginePtr, GlobalPtr::assetManagerPtr, glm::vec3(0.0f, 0.0f, 0.0f))));
    GlobalPtr::assetManagerPtr->ObjManager->ObjectList.emplace_back(gameObject);

    std::shared_ptr<GameObject> gameObject2 = std::make_shared<GameObject>(GameObject(GlobalPtr::enginePtr));
    gameObject2->AddChildModel(GlobalPtr::assetManagerPtr->meshManager, std::make_shared<Model>(Model(GlobalPtr::enginePtr, GlobalPtr::assetManagerPtr->materialManager, GlobalPtr::assetManagerPtr->textureManager, "../Models/TestAnimModel/model.dae")));
    GlobalPtr::assetManagerPtr->ObjManager->ObjectList.emplace_back(gameObject2);

    std::shared_ptr<GameObject> gameObject3 = std::make_shared<GameObject>(GameObject(GlobalPtr::enginePtr));
    gameObject3->AddChildMesh(GlobalPtr::assetManagerPtr->meshManager, std::make_shared<Skybox>(Skybox(GlobalPtr::enginePtr, GlobalPtr::assetManagerPtr)));
    GlobalPtr::assetManagerPtr->ObjManager->ObjectList.emplace_back(gameObject3);


    MaterialTexture material;
    material.DiffuseMap = LoadTexture2D("../texture/forrest_ground_01_diff_4k.jpg", VK_FORMAT_R8G8B8A8_SRGB);
    material.AlbedoMap = LoadTexture2D("../texture/forrest_ground_01_diff_4k.jpg", VK_FORMAT_R8G8B8A8_SRGB);
    material.RoughnessMap = LoadTexture2D("../texture/forrest_ground_01_rough_4k.jpg", VK_FORMAT_R8G8B8A8_UNORM);
    material.AOMap = LoadTexture2D("../texture/forrest_ground_01_ao_4k.jpg", VK_FORMAT_R8G8B8A8_UNORM);
    material.NormalMap = LoadTexture2D("../texture/forrest_ground_01_nor_4k.jpg", VK_FORMAT_R8G8B8A8_UNORM);
    material.DepthMap = LoadTexture2D("../texture/forrest_ground_01_disp_4k.jpg", VK_FORMAT_R8G8B8A8_UNORM);
    material.MatallicMap = LoadTexture2D("../texture/forrest_ground_01_disp_4k.jpg", VK_FORMAT_R8G8B8A8_UNORM);

    std::shared_ptr<Material> materialPtr = LoadMaterial("TerrianMaterial", material);

    std::shared_ptr<GameObject> gameObject4 = std::make_shared<GameObject>(GameObject(GlobalPtr::enginePtr));
    gameObject4->AddChildMesh(GlobalPtr::assetManagerPtr->meshManager, std::make_shared<TerrainMesh>(TerrainMesh(GlobalPtr::enginePtr, "../texture/perlin_noise.png", materialPtr)));
    GlobalPtr::assetManagerPtr->ObjManager->ObjectList.emplace_back(gameObject4);

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

    GlobalPtr::assetManagerPtr->textureManager->Load3DTexture("C:/Users/dotha/Desktop/detailed_surfaces/media/sculptureSphere.dds", VK_FORMAT_R8_UNORM);
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
    GlobalPtr::assetManagerPtr->meshManager->AddMesh(std::make_shared<Skybox>(Skybox(GlobalPtr::enginePtr, GlobalPtr::assetManagerPtr)));
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
    GlobalPtr::assetManagerPtr->ObjManager->ObjectList.emplace_back(std::make_shared<GUIPanel>(GUIPanel(GlobalPtr::enginePtr, materialPtr, rectangle, glm::vec2(0.0f))));
    GlobalPtr::assetManagerPtr->ObjManager->ObjectList.emplace_back(std::make_shared<TextMesh>(TextMesh(GlobalPtr::enginePtr, GlobalPtr::assetManagerPtr->materialManager, GlobalPtr::assetManagerPtr->textureManager, GlobalPtr::assetManagerPtr->guiManager->FontList[0], "SparkMan", glm::vec2(0.0f))));

    GlobalPtr::assetManagerPtr->SceneData->UniformDataInfo.dlight.direction = glm::vec4(0.0f);
    GlobalPtr::assetManagerPtr->SceneData->UniformDataInfo.dlight.ambient = glm::vec4(0.2f);
    GlobalPtr::assetManagerPtr->SceneData->UniformDataInfo.dlight.diffuse = glm::vec4(0.5f);
    GlobalPtr::assetManagerPtr->SceneData->UniformDataInfo.dlight.specular = glm::vec4(1.0f);

    GlobalPtr::assetManagerPtr->SceneData->UniformDataInfo.plight[0].position = glm::vec4(0.5f, 1.0f, 0.3f, 1.0f);
    GlobalPtr::assetManagerPtr->SceneData->UniformDataInfo.plight[0].ambient = glm::vec4(0.2f);
    GlobalPtr::assetManagerPtr->SceneData->UniformDataInfo.plight[0].diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 0.0f);
    GlobalPtr::assetManagerPtr->SceneData->UniformDataInfo.plight[0].specular = glm::vec4(1.0f);

    GlobalPtr::assetManagerPtr->SceneData->UniformDataInfo.plight[1].position = glm::vec4(0.5f, 1.0f, 0.3f, 1.0f);
    GlobalPtr::assetManagerPtr->SceneData->UniformDataInfo.plight[1].ambient = glm::vec4(0.2f);
    GlobalPtr::assetManagerPtr->SceneData->UniformDataInfo.plight[1].diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 0.0f);
    GlobalPtr::assetManagerPtr->SceneData->UniformDataInfo.plight[1].specular = glm::vec4(1.0f);

    GlobalPtr::assetManagerPtr->SceneData->UniformDataInfo.plight[2].position = glm::vec4(0.5f, 1.0f, 0.3f, 1.0f);
    GlobalPtr::assetManagerPtr->SceneData->UniformDataInfo.plight[2].ambient = glm::vec4(0.2f);
    GlobalPtr::assetManagerPtr->SceneData->UniformDataInfo.plight[2].diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 0.0f);
    GlobalPtr::assetManagerPtr->SceneData->UniformDataInfo.plight[2].specular = glm::vec4(1.0f);

    GlobalPtr::assetManagerPtr->SceneData->UniformDataInfo.plight[3].position = glm::vec4(0.5f, 1.0f, 0.3f, 1.0f);
    GlobalPtr::assetManagerPtr->SceneData->UniformDataInfo.plight[3].ambient = glm::vec4(0.2f);
    GlobalPtr::assetManagerPtr->SceneData->UniformDataInfo.plight[3].diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 0.0f);
    GlobalPtr::assetManagerPtr->SceneData->UniformDataInfo.plight[3].specular = glm::vec4(1.0f);

    GlobalPtr::assetManagerPtr->SceneData->UniformDataInfo.plight[4].position = glm::vec4(0.5f, 1.0f, 0.3f, 1.0f);
    GlobalPtr::assetManagerPtr->SceneData->UniformDataInfo.plight[4].ambient = glm::vec4(0.2f);
    GlobalPtr::assetManagerPtr->SceneData->UniformDataInfo.plight[4].diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 0.0f);
    GlobalPtr::assetManagerPtr->SceneData->UniformDataInfo.plight[4].specular = glm::vec4(1.0f);

    GlobalPtr::assetManagerPtr->SceneData->UniformDataInfo.sLight.ambient = glm::vec4(0.0f);
    GlobalPtr::assetManagerPtr->SceneData->UniformDataInfo.sLight.diffuse = glm::vec4(1.0f);
    GlobalPtr::assetManagerPtr->SceneData->UniformDataInfo.sLight.specular = glm::vec4(1.0f);

    renderer = Renderer(GlobalPtr::enginePtr, GlobalPtr::windowPtr, GlobalPtr::assetManagerPtr);
}

Engine::~Engine()
{
    GlobalPtr::assetManagerPtr->Delete(GlobalPtr::enginePtr);
    renderer.Destroy(GlobalPtr::enginePtr);
    GlobalPtr::enginePtr->Destroy();
    GlobalPtr::enginePtr->Destroy();
}

void Engine::MainLoop()
{
    while (!glfwWindowShouldClose(GlobalPtr::windowPtr->GetWindowPtr()))
    {
        glfwPollEvents();

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            renderer.GUIUpdate(GlobalPtr::enginePtr);
        }
        ImGui::Render();

        renderer.Draw(GlobalPtr::enginePtr, GlobalPtr::windowPtr);
    }

    vkDeviceWaitIdle(GlobalPtr::enginePtr->Device);
}

std::shared_ptr<Texture> Engine::LoadTexture2D(const std::string& FilePath, VkFormat format)
{
    return GlobalPtr::assetManagerPtr->textureManager->LoadTexture2D(FilePath, format);
}

std::shared_ptr<Material> Engine::LoadMaterial(const std::string& MaterialName, MaterialTexture& material)
{
    return GlobalPtr::assetManagerPtr->materialManager->LoadMaterial(MaterialName, material);
}

std::shared_ptr<Material> Engine::LoadMaterial(const std::string& MaterialName, std::shared_ptr<Material> material)
{
    return GlobalPtr::assetManagerPtr->materialManager->LoadMaterial(MaterialName, material);
}

void Engine::LoadCubeMap(std::string CubeMapFiles[6])
{
    GlobalPtr::assetManagerPtr->textureManager->LoadCubeMap(CubeMapFiles, VK_FORMAT_R8G8B8A8_UNORM);
}

void Engine::LoadTerrain(const std::string& HeightMapPath, std::shared_ptr<Material> material)
{
    GlobalPtr::assetManagerPtr->meshManager->AddMesh(std::make_shared<TerrainMesh>(TerrainMesh(GlobalPtr::enginePtr, HeightMapPath, material)));
}
