#include "AssetManager.h"

AssetManager::AssetManager()
{
}

AssetManager::AssetManager(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window)
{
    cameraManager = std::make_shared<CameraManager>(CameraManager(engine));
    inputManager = std::make_shared<InputManager>(InputManager(window, cameraManager));
    textureManager = std::make_shared<TextureManager>(TextureManager(engine));
    materialManager = std::make_shared<MaterialManager>(engine, textureManager);
    meshManager = std::make_shared<MeshManager>(MeshManager(engine, inputManager, materialManager));
    lightManager = std::make_shared<LightManager>(engine, cameraManager);
    guiManager = std::make_shared<GUIManager>(engine, materialManager, textureManager, inputManager);
    ObjManager = std::make_shared<ObjectManager>(engine, materialManager, textureManager);

    SceneData = std::make_shared<SceneDataUniformBuffer>(SceneDataUniformBuffer(engine));
    SkyUniformBuffer = std::make_shared<UniformData<SkyboxUniformBuffer>>(engine);

}

AssetManager::~AssetManager()
{
}

void AssetManager::LoadFont(std::shared_ptr<VulkanEngine> engine, const std::string FontLocation)
{
    guiManager->LoadFont(FontLocation);
}

void AssetManager::Update(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window, bool RayTraceFlag)
{
    float timer = engine->VulkanTimer();
    cameraManager->Update();
    inputManager->Update();
    materialManager->Update();
    textureManager->Update();
    meshManager->Update(cameraManager->ActiveCamera);
    lightManager->Update();
    guiManager->Update();
    ObjManager->Update(inputManager, materialManager);
    if (cameraManager->ActiveCamera->cameraType == CameraType::Perspective_Camera)
    {
       // SceneData->UniformDataInfo.sLight.direction = static_cast<PerspectiveCamera*>(ActiveCamera.get())->GetFront();
    }

    SceneData->UniformDataInfo.DirectionalLightCount = lightManager->GetDirectionalLightDescriptorCount();
    SceneData->UniformDataInfo.PointLightCount = lightManager->GetPointLightDescriptorCount();
    SceneData->UniformDataInfo.SpotLightCount = lightManager->GetSpotLightDescriptorCount();
    SceneData->UniformDataInfo.viewInverse = glm::inverse(cameraManager->ActiveCamera->GetViewMatrix());
    SceneData->UniformDataInfo.projInverse = glm::inverse(cameraManager->ActiveCamera->GetProjectionMatrix());
    SceneData->UniformDataInfo.projInverse[1][1] *= -1;
    SceneData->UniformDataInfo.view = cameraManager->ActiveCamera->GetViewMatrix();
    SceneData->UniformDataInfo.proj = cameraManager->ActiveCamera->GetProjectionMatrix();
    SceneData->UniformDataInfo.proj[1][1] *= -1;
    SceneData->UniformDataInfo.viewPos = glm::vec4(cameraManager->ActiveCamera->GetPosition(), 0.0f);
    SceneData->UniformDataInfo.timer = timer;
    SceneData->Update(engine);

    SkyUniformBuffer->UniformDataInfo.viewInverse = glm::inverse(glm::mat4(glm::mat3(cameraManager->ActiveCamera->GetViewMatrix())));
    SkyUniformBuffer->UniformDataInfo.projInverse = glm::inverse(glm::perspective(glm::radians(cameraManager->ActiveCamera->GetZoom()), engine->SwapChain.GetSwapChainResolution().width / (float)engine->SwapChain.GetSwapChainResolution().height, 0.1f, 100.0f));
    SkyUniformBuffer->UniformDataInfo.projInverse[1][1] *= -1;
    SkyUniformBuffer->UniformDataInfo.view = glm::mat4(glm::mat3(cameraManager->ActiveCamera->GetViewMatrix()));
    SkyUniformBuffer->UniformDataInfo.proj = glm::perspective(glm::radians(cameraManager->ActiveCamera->GetZoom()), engine->SwapChain.GetSwapChainResolution().width / (float)engine->SwapChain.GetSwapChainResolution().height, 0.1f, 100.0f);
    SkyUniformBuffer->UniformDataInfo.proj[1][1] *= -1;
    SkyUniformBuffer->UniformDataInfo.viewPos = glm::vec4(cameraManager->ActiveCamera->GetPosition(), 0.0f);
    SkyUniformBuffer->Update(engine);
}

void AssetManager::Draw(VkCommandBuffer commandBuffer, VkRenderPassBeginInfo renderPassInfo, VkPipelineLayout layout, RenderPassID renderPassID, std::shared_ptr<Camera> CameraView)
{
   meshManager->Draw(commandBuffer, renderPassInfo, layout, renderPassID, CameraView);
}

void AssetManager::GUIDraw(VkCommandBuffer& commandBuffer, VkPipelineLayout layout)
{
   ObjManager->GUIDraw(commandBuffer, layout);
}

void AssetManager::Delete(std::shared_ptr<VulkanEngine> engine)
{
    //for (auto& model : modelManager.ModelList)
    //{
    //    model->Destory(engine);
    //}

    SceneData->Destroy(engine);
    SkyUniformBuffer->Destroy(engine);

    meshManager->Destroy();
    textureManager->Destory();
    materialManager->Destory();
    lightManager->Destory();
    ObjManager->Destory();
}

std::vector<std::shared_ptr<Mesh>> AssetManager::GetMeshByType(MeshTypeFlag type)
{
    std::vector<std::shared_ptr<Mesh>> ReturnMeshs;
    for (auto mesh : meshManager->MeshList)
    {
        if (mesh->MeshType == type)
        {
           ReturnMeshs.emplace_back(mesh);
        }
    }
    return ReturnMeshs;
}
