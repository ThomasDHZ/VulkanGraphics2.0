#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


#include "VulkanWindow.h"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <array>
#include <optional>
#include <set>
#include "VulkanEngine.h"
#include "Texture2D.h"
#include "Mesh.h"
#include "InterfaceRenderPass.h"
#include "BlinnPhongPipeline.h"
#include "RenderedDepthTexture.h"
#include "RendererManager.h"
#include "BlinnPhongRasterPass.h"
#include "CameraManager.h"
#include "InputManager.h"
#include "TextureManager.h"
#include "MaterialManager.h"
#include "MeshManager.h"
#include "LightManager.h"
#include "GUIManager.h"
#include "ObjectManager.h"
#include "AssetManager.h"
#include "GameObject.h"
#include "Skybox.h"

class HelloTriangleApplication {
public:
    void run() {
        initVulkan();
        mainLoop();

       AssetManagerPtr::GetAssetPtr()->Delete();
        renderer.Destroy(EnginePtr::GetEnginePtr());
        EnginePtr::GetEnginePtr()->Destroy();
        WindowPtr::GetWindowPtr()->Destroy();
    }

private:
    RendererManager renderer;
    std::shared_ptr<Mesh> Plain;

    void initVulkan() {

        WindowPtr::SetUpPtr(1280, 720, "VulkanEngine");
        EnginePtr::SetUpPtr(WindowPtr::GetWindowPtr());
        CameraManagerPtr::SetUpPtr(EnginePtr::GetEnginePtr());
        InputManagerPtr::SetUpPtr(WindowPtr::GetWindowPtr(), CameraManagerPtr::GetCameraManagerPtr());
        TextureManagerPtr::SetUpPtr(EnginePtr::GetEnginePtr());
        MaterialManagerPtr::SetUpPtr(EnginePtr::GetEnginePtr());
        MeshManagerPtr::SetUpPtr(EnginePtr::GetEnginePtr(), InputManagerPtr::GetInputManagerPtr(), MaterialManagerPtr::GetMaterialManagerPtr());
        LightManagerPtr::SetUpPtr(EnginePtr::GetEnginePtr(), CameraManagerPtr::GetCameraManagerPtr());
        GuiManagerPtr::SetUpPtr(EnginePtr::GetEnginePtr());
        ObjManagerPtr::SetUpPtr(EnginePtr::GetEnginePtr());
        AssetManagerPtr::SetUpPtr(EnginePtr::GetEnginePtr());

        //std::string CubeMapFiles[6];
        //CubeMapFiles[0] = "../texture/skybox/right.jpg";
        //CubeMapFiles[1] = "../texture/skybox/left.jpg";
        //CubeMapFiles[2] = "../texture/skybox/top.jpg";
        //CubeMapFiles[3] = "../texture/skybox/bottom.jpg";
        //CubeMapFiles[4] = "../texture/skybox/back.jpg";
        //CubeMapFiles[5] = "../texture/skybox/front.jpg";

        //std::string CubeMapFiles[6];
        //CubeMapFiles[0] = "../texture/skybox/CosmicCoolCloudLeft.png";
        //CubeMapFiles[1] = "../texture/skybox/CosmicCoolCloudRight.png";
        //CubeMapFiles[2] = "../texture/skybox/CosmicCoolCloudTop.png";
        //CubeMapFiles[3] = "../texture/skybox/CosmicCoolCloudBottom.png";
        //CubeMapFiles[4] = "../texture/skybox/CosmicCoolCloudBack.png";
        //CubeMapFiles[5] = "../texture/skybox/CosmicCoolCloudFront.png";

        std::string CubeMapFiles[6];
        CubeMapFiles[0] = "../texture/skybox/hot_lf.png";
        CubeMapFiles[1] = "../texture/skybox/hot_rt.png";
        CubeMapFiles[2] = "../texture/skybox/hot_up.png";
        CubeMapFiles[3] = "../texture/skybox/hot_dn.png";
        CubeMapFiles[4] = "../texture/skybox/hot_bk.png";
        CubeMapFiles[5] = "../texture/skybox/hot_ft.png";

        AssetManagerPtr::GetAssetPtr()->textureManager->LoadCubeMap(CubeMapFiles, VK_FORMAT_R8G8B8A8_UNORM);
        AssetManagerPtr::GetAssetPtr()->textureManager->LoadCubeMap(CubeMapFiles, VK_FORMAT_R8G8B8A8_UNORM);
        auto a = AssetManagerPtr::GetAssetPtr()->textureManager->GetAllCubeMapTextures();
        AssetManagerPtr::GetAssetPtr()->meshManager->AddMesh(std::make_shared<Skybox>(Skybox(EnginePtr::GetEnginePtr(), AssetManagerPtr::GetAssetPtr())));

        renderer = RendererManager(EnginePtr::GetEnginePtr(), WindowPtr::GetWindowPtr());

 /*       PointLightBuffer plight = PointLightBuffer();
        plight.position = glm::vec4(-2.0f, 4.0f, -1.0f, 0.0f);
        plight.ambient = glm::vec4(0.2f);
        plight.diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        plight.specular = glm::vec4(1.0f);
        LightManagerPtr::GetLightManagerPtr()->AddPointLight(plight);*/
        DirectionalLightBuffer dlight = DirectionalLightBuffer();
        dlight.direction = glm::vec4(1.0f);
        dlight.ambient = glm::vec4(0.2f);
        dlight.diffuse = glm::vec4(0.5f);
        dlight.specular = glm::vec4(1.0f);
        LightManagerPtr::GetLightManagerPtr()->AddDirectionalLight(CameraManagerPtr::GetCameraManagerPtr(), dlight);
        LightManagerPtr::GetLightManagerPtr()->AddDirectionalLight(CameraManagerPtr::GetCameraManagerPtr(), dlight);

        PointLightBuffer plight = PointLightBuffer();
        plight.position = glm::vec4(-10.0f, 10.0f, 10.0f, 0.0f);
        plight.ambient = glm::vec4(0.2f);
        plight.diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        plight.specular = glm::vec4(1.0f);

        PointLightBuffer plight2 = PointLightBuffer();
        plight2.position = glm::vec4(10.0f, 10.0f, 10.0f, 0.0f);
        plight2.ambient = glm::vec4(0.2f);
        plight2.diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        plight2.specular = glm::vec4(1.0f);

        PointLightBuffer plight3 = PointLightBuffer();
        plight3.position = glm::vec4(-10.0f, -10.0f, 10.0f, 0.0f);
        plight3.ambient = glm::vec4(0.2f);
        plight3.diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        plight3.specular = glm::vec4(1.0f);

        PointLightBuffer plight4 = PointLightBuffer();
        plight4.position = glm::vec4(10.0f, -10.0f, 10.0f, 0.0f);
        plight4.ambient = glm::vec4(0.2f);
        plight4.diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        plight4.specular = glm::vec4(1.0f);

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
        LightManagerPtr::GetLightManagerPtr()->AddPointLight(plight2);
        LightManagerPtr::GetLightManagerPtr()->AddPointLight(plight3);
        LightManagerPtr::GetLightManagerPtr()->AddPointLight(plight4);
        LightManagerPtr::GetLightManagerPtr()->AddSphereAreaLight(spherelight);
        LightManagerPtr::GetLightManagerPtr()->AddSpotLight(slight);

       // std::vector<Vertex> vertices =
       // {
       //     {{-1.0f, -1.0f, -1.0f},   { 0.0f }, {0.0f,  0.0f, -1.0f}, { 0.0f }, {0.0f,  0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
       //     {{ 1.0f,  1.0f, -1.0f},   { 0.0f }, {0.0f,  0.0f, -1.0f}, { 0.0f }, {1.0f, 1.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
       //     {{ 1.0f, -1.0f, -1.0f},   { 0.0f }, {0.0f,  0.0f, -1.0f}, { 0.0f }, {1.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
       //     { { 1.0f,  1.0f, -1.0f},  { 0.0f }, {0.0f,  0.0f, -1.0f}, { 0.0f }, {1.0f, 1.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
       //     { {-1.0f, -1.0f, -1.0f},  { 0.0f }, {0.0f,  0.0f, -1.0f}, { 0.0f }, {0.0f,  0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
       //     { {-1.0f,  1.0f, -1.0f},  { 0.0f }, {0.0f,  0.0f, -1.0f}, { 0.0f }, {0.0f, 1.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},

       //     {{-1.0f, -1.0f,  1.0f},   { 0.0f }, {0.0f,  0.0f, 1.0f}, { 0.0f }, {0.0f,  0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
       //     {{ 1.0f, -1.0f,  1.0f},   { 0.0f }, {0.0f,  0.0f, 1.0f}, { 0.0f }, {1.0f, 1.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
       //     {{ 1.0f,  1.0f,  1.0f},   { 0.0f }, {0.0f,  0.0f, 1.0f}, { 0.0f }, {1.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
       //     { { 1.0f,  1.0f,  1.0f},  { 0.0f }, {0.0f,  0.0f, 1.0f}, { 0.0f }, {1.0f, 1.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
       //     { {-1.0f,  1.0f,  1.0f},  { 0.0f }, {0.0f,  0.0f, 1.0f}, { 0.0f }, {0.0f,  0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
       //     { {-1.0f, -1.0f,  1.0f},  { 0.0f }, {0.0f,  0.0f, 1.0f}, { 0.0f }, {0.0f, 1.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},

       //     {{-1.0f,  1.0f,  1.0f},   { 0.0f }, {-1.0f,  0.0f,  0.0f}, { 0.0f }, {0.0f,  0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
       //     {{ -1.0f,  1.0f, -1.0f},   { 0.0f }, {-1.0f,  0.0f,  0.0f}, { 0.0f }, {1.0f, 1.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
       //     {{-1.0f, -1.0f, -1.0f},   { 0.0f }, {-1.0f,  0.0f,  0.0f}, { 0.0f }, {1.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
       //     { {-1.0f, -1.0f, -1.0f},  { 0.0f }, {-1.0f,  0.0f,  0.0f}, { 0.0f }, {1.0f, 1.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
       //     { {-1.0f, -1.0f,  1.0f},  { 0.0f }, {-1.0f,  0.0f,  0.0f}, { 0.0f }, {0.0f,  0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
       //     { {-1.0f,  1.0f,  1.0f},  { 0.0f }, {-1.0f,  0.0f,  0.0f}, { 0.0f }, {0.0f, 1.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},

       //     {{1.0f,  1.0f,  1.0f},   { 0.0f }, {1.0f,  0.0f,  0.0f}, { 0.0f }, {0.0f,  0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
       //     {{1.0f, -1.0f, -1.0f},   { 0.0f }, {1.0f,  0.0f,  0.0f}, { 0.0f }, {1.0f, 1.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
       //     {{ 1.0f,  1.0f, -1.0f},   { 0.0f }, {1.0f,  0.0f,  0.0f}, { 0.0f }, {1.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
       //     { {1.0f, -1.0f, -1.0f},  { 0.0f }, {1.0f,  0.0f,  0.0f}, { 0.0f }, {1.0f, 1.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
       //     { {1.0f,  1.0f,  1.0f},  { 0.0f }, {1.0f,  0.0f,  0.0f}, { 0.0f }, {0.0f,  0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
       //     { { 1.0f, -1.0f,  1.0f},  { 0.0f }, {1.0f,  0.0f,  0.0f}, { 0.0f }, {0.0f, 1.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},

       //     {{-1.0f, -1.0f, -1.0f},   { 0.0f }, {0.0f, -1.0f,  0.0f}, { 0.0f }, {0.0f,  0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
       //     {{1.0f, -1.0f, -1.0f},   { 0.0f }, {0.0f, -1.0f,  0.0f}, { 0.0f }, {1.0f, 1.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
       //     {{1.0f, -1.0f,  1.0f},   { 0.0f }, {0.0f, -1.0f,  0.0f}, { 0.0f }, {1.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
       //     { {1.0f, -1.0f,  1.0f},  { 0.0f }, {0.0f, -1.0f,  0.0f}, { 0.0f }, {1.0f, 1.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
       //     { {-1.0f, -1.0f,  1.0f},  { 0.0f }, {0.0f, -1.0f,  0.0f}, { 0.0f }, {0.0f,  0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
       //     { {-1.0f, -1.0f, -1.0f},  { 0.0f }, {0.0f, -1.0f,  0.0f}, { 0.0f }, {0.0f, 1.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},

       //     {{-1.0f,  1.0f, -1.0f},   { 0.0f }, {0.0f,  1.0f,  0.0f}, { 0.0f }, {0.0f,  0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
       //     {{1.0f,  1.0f , 1.0f},   { 0.0f }, {0.0f,  1.0f,  0.0f}, { 0.0f }, {1.0f, 1.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
       //     {{1.0f,  1.0f, -1.0f},   { 0.0f }, {0.0f,  1.0f,  0.0f}, { 0.0f }, {1.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
       //     { {1.0f,  1.0f,  1.0f},  { 0.0f }, {0.0f,  1.0f,  0.0f}, { 0.0f }, {1.0f, 1.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
       //     { {-1.0f,  1.0f, -1.0f},  { 0.0f }, {0.0f,  1.0f,  0.0f}, { 0.0f }, {0.0f,  0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
       //     { {-1.0f,  1.0f,  1.0f},  { 0.0f }, {0.0f,  1.0f,  0.0f}, { 0.0f }, {0.0f, 1.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
       // };

       // MaterialTexture material;
       // material.DiffuseMap = TextureManagerPtr::GetTextureManagerPtr()->LoadTexture2D("C:/Users/dotha/source/repos/VulkanGraphics/texture/wood.png", VK_FORMAT_R8G8B8A8_SRGB);
       // material.AlbedoMap = TextureManagerPtr::GetTextureManagerPtr()->LoadTexture2D("C:/Users/dotha/source/repos/VulkanGraphics/texture/wood.png", VK_FORMAT_R8G8B8A8_SRGB);


       //{
       //     std::shared_ptr<GameObject> gameObject = std::make_shared<GameObject>(GameObject(EnginePtr::GetEnginePtr()));
       //     gameObject->AddChildMesh(std::make_shared<Mesh>(vertices));
       //     AssetManagerPtr::GetAssetPtr()->ObjManager->ObjectList.emplace_back(gameObject);
       //     gameObject->MeshList[0]->MeshPosition = glm::vec3(0.0f, 1.5f, 0.0f);
       //     gameObject->MeshList[0]->MeshScale = glm::vec3(0.5f);

       //     auto mat = MaterialManagerPtr::GetMaterialManagerPtr()->LoadMaterial("Cube", material);
       //     gameObject->MeshList[0]->MeshMaterial = mat;
       // }
       // {
       //     std::shared_ptr<GameObject> gameObject = std::make_shared<GameObject>(GameObject(EnginePtr::GetEnginePtr()));
       //     gameObject->AddChildMesh(std::make_shared<Mesh>(vertices));
       //     AssetManagerPtr::GetAssetPtr()->ObjManager->ObjectList.emplace_back(gameObject);
       //     gameObject->MeshList[0]->MeshPosition = glm::vec3(2.0f, 0.0f, 1.0);
       //     gameObject->MeshList[0]->MeshScale = glm::vec3(0.5f);

       //     auto mat = MaterialManagerPtr::GetMaterialManagerPtr()->LoadMaterial("Cube", material);
       //     gameObject->MeshList[0]->MeshMaterial = mat;
       // }
       // {
       //     std::shared_ptr<GameObject> gameObject = std::make_shared<GameObject>(GameObject(EnginePtr::GetEnginePtr()));
       //     gameObject->AddChildMesh(std::make_shared<Mesh>(vertices));
       //     AssetManagerPtr::GetAssetPtr()->ObjManager->ObjectList.emplace_back(gameObject);
       //     gameObject->MeshList[0]->MeshPosition = glm::vec3(-1.0f, 0.0f, 2.0);
       //     gameObject->MeshList[0]->MeshRotation = glm::vec3(60.0f, 0.0f, 60.0);
       //     gameObject->MeshList[0]->MeshScale = glm::vec3(0.25f);

       //     auto mat = MaterialManagerPtr::GetMaterialManagerPtr()->LoadMaterial("Cube", material);
       //     gameObject->MeshList[0]->MeshMaterial = mat;
       // }
       // {
       //     std::vector<Vertex> floorvertices =
       //     {
       //         {{25.0f, -0.5f,  25.0f }, { 0.0f }, {0.0f, 1.0f, 0.0f}, { 0.0f }, {25.0f,  0.0f}, { 25.0f,  0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
       //         {{ -25.0f, -0.5f,  25.0f }, { 0.0f }, {0.0f, 1.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f}, { 0.0f, 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
       //         {{ -25.0f, -0.5f, -25.0f }, { 0.0f }, {0.0f, 1.0f, 0.0f}, { 0.0f }, {0.0f, 25.0f}, { 0.0f, 25.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
       //         {{25.0f, -0.5f,  25.0f }, { 0.0f }, {0.0f, 1.0f, 0.0f}, { 0.0f }, {25.0f,  0.0f}, {25.0f,  0.0f}, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
       //         {{ -25.0f, -0.5f, -25.0f }, { 0.0f }, {0.0f, 1.0f, 0.0f}, { 0.0f }, {0.0f, 25.0f }, { 0.0f, 25.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
       //         {{25.0f, -0.5f, -25.0f }, { 0.0f }, {0.0f, 1.0f, 0.0f}, { 0.0f }, {25.0f, 25.0f}, {25.0f, 25.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f}, { 0.0f }},
       //     };
       //     std::shared_ptr<GameObject> gameObject = std::make_shared<GameObject>(GameObject(EnginePtr::GetEnginePtr()));
       //     gameObject->AddChildMesh(std::make_shared<Mesh>(floorvertices));
       //     AssetManagerPtr::GetAssetPtr()->ObjManager->ObjectList.emplace_back(gameObject);

       //     MaterialTexture material;
       //     material.DiffuseMap = TextureManagerPtr::GetTextureManagerPtr()->LoadTexture2D("C:/Users/dotha/source/repos/VulkanGraphics/texture/wood.png", VK_FORMAT_R8G8B8A8_SRGB);
       //     material.AlbedoMap = TextureManagerPtr::GetTextureManagerPtr()->LoadTexture2D("C:/Users/dotha/source/repos/VulkanGraphics/texture/wood.png", VK_FORMAT_R8G8B8A8_SRGB);

       //     auto mat = MaterialManagerPtr::GetMaterialManagerPtr()->LoadMaterial("Cube", material);
       //     gameObject->MeshList[0]->MeshMaterial = mat;
       // }

                std::shared_ptr<GameObject> gameObject = std::make_shared<GameObject>(GameObject(EnginePtr::GetEnginePtr()));
        gameObject->AddChildModel(std::make_shared<Model>(Model("../Models/vulkanscene_shadow.obj")));
        AssetManagerPtr::GetAssetPtr()->ObjManager->ObjectList.emplace_back(gameObject);

        //std::shared_ptr<GameObject> gameObject = std::make_shared<GameObject>(GameObject(EnginePtr::GetEnginePtr()));
        //gameObject->AddChildModel(std::make_shared<Model>(Model("../Models/RayReflectionTest.obj")));
        //AssetManagerPtr::GetAssetPtr()->ObjManager->ObjectList.emplace_back(gameObject);


        //std::shared_ptr<GameObject> gameObject = std::make_shared<GameObject>(GameObject(EnginePtr::GetEnginePtr()));
        //gameObject->AddChildModel(std::make_shared<Model>(Model("../Models/Sponza/Sponza.gltf")));
        //AssetManagerPtr::GetAssetPtr()->ObjManager->ObjectList.emplace_back(gameObject);

       /* {
            std::shared_ptr<GameObject> gameObject = std::make_shared<GameObject>(GameObject(EnginePtr::GetEnginePtr()));
            gameObject->AddChildModel(std::make_shared<Model>(Model("../Models/sphere.obj")));
            AssetManagerPtr::GetAssetPtr()->ObjManager->ObjectList.emplace_back(gameObject);


            MaterialTexture material;
            material.DiffuseMap = TextureManagerPtr::GetTextureManagerPtr()->LoadTexture2D("C:/Users/dotha/source/repos/VulkanGraphics/texture/pbr/rusted_iron/albedo.png", VK_FORMAT_R8G8B8A8_SRGB);
            material.AlbedoMap = TextureManagerPtr::GetTextureManagerPtr()->LoadTexture2D("C:/Users/dotha/source/repos/VulkanGraphics/texture/pbr/rusted_iron/albedo.png", VK_FORMAT_R8G8B8A8_SRGB);
            material.MatallicMap = TextureManagerPtr::GetTextureManagerPtr()->LoadTexture2D("C:/Users/dotha/source/repos/VulkanGraphics/texture/pbr/rusted_iron/metallic.png", VK_FORMAT_R8G8B8A8_UNORM);
            material.RoughnessMap = TextureManagerPtr::GetTextureManagerPtr()->LoadTexture2D("C:/Users/dotha/source/repos/VulkanGraphics/texture/pbr/rusted_iron/roughness.png", VK_FORMAT_R8G8B8A8_UNORM);
            material.AOMap = TextureManagerPtr::GetTextureManagerPtr()->LoadTexture2D("C:/Users/dotha/source/repos/VulkanGraphics/texture/pbr/rusted_iron/ao.png", VK_FORMAT_R8G8B8A8_UNORM);
            material.NormalMap = TextureManagerPtr::GetTextureManagerPtr()->LoadTexture2D("C:/Users/dotha/source/repos/VulkanGraphics/texture/pbr/rusted_iron/normal.png", VK_FORMAT_R8G8B8A8_UNORM);

            auto mat = MaterialManagerPtr::GetMaterialManagerPtr()->LoadMaterial("RustedIron", material);
            gameObject->MeshList[0]->MeshMaterial = mat;
            gameObject->MeshList[0]->MeshPosition = glm::vec3(-6.0f, 0.0f, 0.0f);
        }
        {
            std::shared_ptr<GameObject> gameObject = std::make_shared<GameObject>(GameObject(EnginePtr::GetEnginePtr()));
            gameObject->AddChildModel(std::make_shared<Model>(Model("../Models/sphere.obj")));
            AssetManagerPtr::GetAssetPtr()->ObjManager->ObjectList.emplace_back(gameObject);


            MaterialTexture material;
            material.DiffuseMap = TextureManagerPtr::GetTextureManagerPtr()->LoadTexture2D("C:/Users/dotha/source/repos/VulkanGraphics/texture/pbr/grass/albedo.png", VK_FORMAT_R8G8B8A8_SRGB);
            material.AlbedoMap = TextureManagerPtr::GetTextureManagerPtr()->LoadTexture2D("C:/Users/dotha/source/repos/VulkanGraphics/texture/pbr/grass/albedo.png", VK_FORMAT_R8G8B8A8_SRGB);
            material.MatallicMap = TextureManagerPtr::GetTextureManagerPtr()->LoadTexture2D("C:/Users/dotha/source/repos/VulkanGraphics/texture/pbr/grass/metallic.png", VK_FORMAT_R8G8B8A8_UNORM);
            material.RoughnessMap = TextureManagerPtr::GetTextureManagerPtr()->LoadTexture2D("C:/Users/dotha/source/repos/VulkanGraphics/texture/pbr/grass/roughness.png", VK_FORMAT_R8G8B8A8_UNORM);
            material.AOMap = TextureManagerPtr::GetTextureManagerPtr()->LoadTexture2D("C:/Users/dotha/source/repos/VulkanGraphics/texture/pbr/grass/ao.png", VK_FORMAT_R8G8B8A8_UNORM);
            material.NormalMap = TextureManagerPtr::GetTextureManagerPtr()->LoadTexture2D("C:/Users/dotha/source/repos/VulkanGraphics/texture/pbr/grass/normal.png", VK_FORMAT_R8G8B8A8_UNORM);

            auto mat = MaterialManagerPtr::GetMaterialManagerPtr()->LoadMaterial("grass", material);
            gameObject->MeshList[0]->MeshMaterial = mat;
            gameObject->MeshList[0]->MeshPosition = glm::vec3(-3.0f, 0.0f, 0.0f);
        }
        {
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

            auto mat = MaterialManagerPtr::GetMaterialManagerPtr()->LoadMaterial("Gold", material);
            gameObject->MeshList[0]->MeshMaterial = mat;
            gameObject->MeshList[0]->MeshPosition = glm::vec3(0.0f, 0.0f, 0.0f);
        }
        {
            std::shared_ptr<GameObject> gameObject = std::make_shared<GameObject>(GameObject(EnginePtr::GetEnginePtr()));
            gameObject->AddChildModel(std::make_shared<Model>(Model("../Models/sphere.obj")));
            AssetManagerPtr::GetAssetPtr()->ObjManager->ObjectList.emplace_back(gameObject);


            MaterialTexture material;
            material.DiffuseMap = TextureManagerPtr::GetTextureManagerPtr()->LoadTexture2D("C:/Users/dotha/source/repos/VulkanGraphics/texture/pbr/plastic/albedo.png", VK_FORMAT_R8G8B8A8_SRGB);
            material.AlbedoMap = TextureManagerPtr::GetTextureManagerPtr()->LoadTexture2D("C:/Users/dotha/source/repos/VulkanGraphics/texture/pbr/plastic/albedo.png", VK_FORMAT_R8G8B8A8_SRGB);
            material.MatallicMap = TextureManagerPtr::GetTextureManagerPtr()->LoadTexture2D("C:/Users/dotha/source/repos/VulkanGraphics/texture/pbr/plastic/metallic.png", VK_FORMAT_R8G8B8A8_UNORM);
            material.RoughnessMap = TextureManagerPtr::GetTextureManagerPtr()->LoadTexture2D("C:/Users/dotha/source/repos/VulkanGraphics/texture/pbr/plastic/roughness.png", VK_FORMAT_R8G8B8A8_UNORM);
            material.AOMap = TextureManagerPtr::GetTextureManagerPtr()->LoadTexture2D("C:/Users/dotha/source/repos/VulkanGraphics/texture/pbr/plastic/ao.png", VK_FORMAT_R8G8B8A8_UNORM);
            material.NormalMap = TextureManagerPtr::GetTextureManagerPtr()->LoadTexture2D("C:/Users/dotha/source/repos/VulkanGraphics/texture/pbr/plastic/normal.png", VK_FORMAT_R8G8B8A8_UNORM);

            auto mat = MaterialManagerPtr::GetMaterialManagerPtr()->LoadMaterial("sphere", material);
            gameObject->MeshList[0]->MeshMaterial = mat;
            gameObject->MeshList[0]->MeshPosition = glm::vec3(3.0f, 0.0f, 0.0f);
        }
        {
            std::shared_ptr<GameObject> gameObject = std::make_shared<GameObject>(GameObject(EnginePtr::GetEnginePtr()));
            gameObject->AddChildModel(std::make_shared<Model>(Model("../Models/sphere.obj")));
            AssetManagerPtr::GetAssetPtr()->ObjManager->ObjectList.emplace_back(gameObject);


            MaterialTexture material;
            material.DiffuseMap = TextureManagerPtr::GetTextureManagerPtr()->LoadTexture2D("C:/Users/dotha/source/repos/VulkanGraphics/texture/pbr/wall/albedo.png", VK_FORMAT_R8G8B8A8_SRGB);
            material.AlbedoMap = TextureManagerPtr::GetTextureManagerPtr()->LoadTexture2D("C:/Users/dotha/source/repos/VulkanGraphics/texture/pbr/wall/albedo.png", VK_FORMAT_R8G8B8A8_SRGB);
            material.MatallicMap = TextureManagerPtr::GetTextureManagerPtr()->LoadTexture2D("C:/Users/dotha/source/repos/VulkanGraphics/texture/pbr/wall/metallic.png", VK_FORMAT_R8G8B8A8_UNORM);
            material.RoughnessMap = TextureManagerPtr::GetTextureManagerPtr()->LoadTexture2D("C:/Users/dotha/source/repos/VulkanGraphics/texture/pbr/wall/roughness.png", VK_FORMAT_R8G8B8A8_UNORM);
            material.AOMap = TextureManagerPtr::GetTextureManagerPtr()->LoadTexture2D("C:/Users/dotha/source/repos/VulkanGraphics/texture/pbr/wall/ao.png", VK_FORMAT_R8G8B8A8_UNORM);
            material.NormalMap = TextureManagerPtr::GetTextureManagerPtr()->LoadTexture2D("C:/Users/dotha/source/repos/VulkanGraphics/texture/pbr/wall/normal.png", VK_FORMAT_R8G8B8A8_UNORM);

            auto mat = MaterialManagerPtr::GetMaterialManagerPtr()->LoadMaterial("wall", material);
            gameObject->MeshList[0]->MeshMaterial = mat;
            gameObject->MeshList[0]->MeshPosition = glm::vec3(6.0f, 0.0f, 0.0f);
        }
        {
            std::shared_ptr<GameObject> gameObject = std::make_shared<GameObject>(GameObject(EnginePtr::GetEnginePtr()));
            gameObject->AddChildModel(std::make_shared<Model>(Model("../Models/plain.obj")));
            AssetManagerPtr::GetAssetPtr()->ObjManager->ObjectList.emplace_back(gameObject);


            MaterialTexture material;
            material.DiffuseMap = TextureManagerPtr::GetTextureManagerPtr()->LoadTexture2D("C:/Users/dotha/source/repos/VulkanGraphics/texture/pbr/grass/albedo.png", VK_FORMAT_R8G8B8A8_SRGB);
            material.AlbedoMap = TextureManagerPtr::GetTextureManagerPtr()->LoadTexture2D("C:/Users/dotha/source/repos/VulkanGraphics/texture/pbr/grass/albedo.png", VK_FORMAT_R8G8B8A8_SRGB);
            material.MatallicMap = TextureManagerPtr::GetTextureManagerPtr()->LoadTexture2D("C:/Users/dotha/source/repos/VulkanGraphics/texture/pbr/grass/metallic.png", VK_FORMAT_R8G8B8A8_UNORM);
            material.RoughnessMap = TextureManagerPtr::GetTextureManagerPtr()->LoadTexture2D("C:/Users/dotha/source/repos/VulkanGraphics/texture/pbr/grass/roughness.png", VK_FORMAT_R8G8B8A8_UNORM);
            material.AOMap = TextureManagerPtr::GetTextureManagerPtr()->LoadTexture2D("C:/Users/dotha/source/repos/VulkanGraphics/texture/pbr/grass/ao.png", VK_FORMAT_R8G8B8A8_UNORM);
            material.NormalMap = TextureManagerPtr::GetTextureManagerPtr()->LoadTexture2D("C:/Users/dotha/source/repos/VulkanGraphics/texture/pbr/grass/normal.png", VK_FORMAT_R8G8B8A8_UNORM);

            auto mat = MaterialManagerPtr::GetMaterialManagerPtr()->LoadMaterial("Cube", material);
            gameObject->MeshList[0]->MeshMaterial = mat;
            gameObject->MeshList[0]->MeshPosition = glm::vec3(0.0f, 0.0f, 0.0f);
        }*/
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(WindowPtr::GetWindowPtr()->GetWindowPtr())) {
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
};

int main() {
    HelloTriangleApplication app;

    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}