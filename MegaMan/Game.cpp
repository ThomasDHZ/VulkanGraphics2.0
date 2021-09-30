#include "Game.h"

Game::Game()
{
    engine.initVulkan();

    std::string CubeMapFiles[6];
    CubeMapFiles[0] = "../texture/skybox/right.jpg";
    CubeMapFiles[1] = "../texture/skybox/left.jpg";
    CubeMapFiles[2] = "../texture/skybox/top.jpg";
    CubeMapFiles[3] = "../texture/skybox/bottom.jpg";
    CubeMapFiles[4] = "../texture/skybox/back.jpg";
    CubeMapFiles[5] = "../texture/skybox/front.jpg";
    AssetManagerPtr::GetAssetPtr()->textureManager->LoadCubeMap(CubeMapFiles, VK_FORMAT_R8G8B8A8_UNORM);

    DirectionalLightBuffer dlight = DirectionalLightBuffer();
    dlight.direction = glm::vec4(1.0f);
    dlight.ambient = glm::vec4(0.2f);
    dlight.diffuse = glm::vec4(0.5f);
    dlight.specular = glm::vec4(1.0f);
    LightManagerPtr::GetLightManagerPtr()->AddDirectionalLight(CameraManagerPtr::GetCameraManagerPtr(), dlight);
    LightManagerPtr::GetLightManagerPtr()->AddDirectionalLight(CameraManagerPtr::GetCameraManagerPtr(), dlight);

    PointLightBuffer plight = PointLightBuffer();
    plight.position = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    plight.ambient = glm::vec4(0.2f);
    plight.diffuse = glm::vec4(0.8f, 0.0f, 0.0f, 1.0f);
    plight.specular = glm::vec4(1.0f);

    SphereAreaLightBuffer spherelight = SphereAreaLightBuffer();
    spherelight.position = glm::vec4(0.5f, 1.0f, 0.3f, 1.0f);
    spherelight.ambient = glm::vec4(0.2f);
    spherelight.diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 0.0f);
    spherelight.specular = glm::vec4(1.0f);
    spherelight.SphereRadius = 10.0f;

    SpotLightBuffer slight = SpotLightBuffer();
    slight.position = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    slight.ambient = glm::vec4(0.2f);
    slight.diffuse = glm::vec4(0.8f, 0.0f, 0.0f, 1.0f);
    slight.specular = glm::vec4(1.0f);

    LightManagerPtr::GetLightManagerPtr()->AddPointLight(plight);
    LightManagerPtr::GetLightManagerPtr()->AddPointLight(plight);
    LightManagerPtr::GetLightManagerPtr()->AddSphereAreaLight(spherelight);
    LightManagerPtr::GetLightManagerPtr()->AddSpotLight(slight);

    std::shared_ptr<GameObject> gameObject = std::make_shared<GameObject>(GameObject(EnginePtr::GetEnginePtr()));
    gameObject->AddChildModel(std::make_shared<Model>(Model("../Models/sphere.obj")));
    AssetManagerPtr::GetAssetPtr()->ObjManager->ObjectList.emplace_back(gameObject);


    MaterialTexture material;
    material.DiffuseMap = TextureManagerPtr::GetTextureManagerPtr()->LoadTexture2D("C:/Users/dotha/source/repos/VulkanGraphics/texture/pbr/gold/albedo.png", VK_FORMAT_R8G8B8A8_SRGB);
    material.AlbedoMap = TextureManagerPtr::GetTextureManagerPtr()->LoadTexture2D("C:/Users/dotha/source/repos/VulkanGraphics/texture/pbr/gold/albedo.png", VK_FORMAT_R8G8B8A8_SRGB);
    material.MatallicMap = TextureManagerPtr::GetTextureManagerPtr()->LoadTexture2D("C:/Users/dotha/source/repos/VulkanGraphics/texture/pbr/gold/metallic.png", VK_FORMAT_R8G8B8A8_UNORM);
    material.RoughnessMap = TextureManagerPtr::GetTextureManagerPtr()->LoadTexture2D("C:/Users/dotha/source/repos/VulkanGraphics/texture/pbr/gold/roughness.png", VK_FORMAT_R8G8B8A8_UNORM);
    material.AOMap = TextureManagerPtr::GetTextureManagerPtr()->LoadTexture2D("C:/Users/dotha/source/repos/VulkanGraphics/texture/pbr/gold/ao.png", VK_FORMAT_R8G8B8A8_UNORM);
    material.NormalMap = TextureManagerPtr::GetTextureManagerPtr()->LoadTexture2D("C:/Users/dotha/source/repos/VulkanGraphics/texture/pbr/gold/normal.png", VK_FORMAT_R8G8B8A8_UNORM);

    auto mat = MaterialManagerPtr::GetMaterialManagerPtr()->LoadMaterial("RustedIron", material);
    gameObject->MeshList[0]->MeshMaterial = mat;

    engine.run();
}

Game::~Game()
{
}
