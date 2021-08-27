#include "AssetManager.h"

std::shared_ptr<AssetManager> AssetManagerPtr::assetPtr = nullptr;

AssetManager::AssetManager()
{
}

AssetManager::AssetManager(std::shared_ptr<VulkanEngine> engine)
{
    cameraManager = CameraManagerPtr::GetCameraManagerPtr();
    inputManager = InputManagerPtr::GetInputManagerPtr();
    textureManager = TextureManagerPtr::GetTextureManagerPtr();
    materialManager = MaterialManagerPtr::GetMaterialManagerPtr();
    meshManager = MeshManagerPtr::GetMeshManagerPtr();
    lightManager = LightManagerPtr::GetLightManagerPtr();
    guiManager = GuiManagerPtr::GetGuiManagerPtr();
    ObjManager = ObjManagerPtr::GetObjManagerPtr();

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

void AssetManager::Update()
{
    float timer = EnginePtr::GetEnginePtr()->VulkanTimer();
    cameraManager->Update();
    inputManager->Update();
    meshManager->Update(cameraManager->ActiveCamera);
    lightManager->Update();
    guiManager->Update();
    ObjManager->Update();

    SceneData->UniformDataInfo.model = glm::rotate(glm::mat4(1.0f), timer * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
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
    SceneData->Update();

    SkyUniformBuffer->UniformDataInfo.viewInverse = glm::inverse(glm::mat4(glm::mat3(cameraManager->ActiveCamera->GetViewMatrix())));
    SkyUniformBuffer->UniformDataInfo.projInverse = glm::inverse(glm::perspective(glm::radians(cameraManager->ActiveCamera->GetZoom()), EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainResolution().width / (float)EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainResolution().height, 0.1f, 100.0f));
    SkyUniformBuffer->UniformDataInfo.projInverse[1][1] *= -1;
    SkyUniformBuffer->UniformDataInfo.view = glm::mat4(glm::mat3(cameraManager->ActiveCamera->GetViewMatrix()));
    SkyUniformBuffer->UniformDataInfo.proj = glm::perspective(glm::radians(cameraManager->ActiveCamera->GetZoom()), EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainResolution().width / (float)EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainResolution().height, 0.1f, 100.0f);
    SkyUniformBuffer->UniformDataInfo.proj[1][1] *= -1;
    SkyUniformBuffer->UniformDataInfo.viewPos = glm::vec4(cameraManager->ActiveCamera->GetPosition(), 0.0f);
    SkyUniformBuffer->Update();
}

void AssetManager::UpdateBufferIndex()
{
    TextureManagerPtr::GetTextureManagerPtr()->UpdateBufferIndex();
    MaterialManagerPtr::GetMaterialManagerPtr()->UpdateBufferIndex();
    MeshManagerPtr::GetMeshManagerPtr()->UpdateBufferIndex();
}

void AssetManager::Draw(VkCommandBuffer commandBuffer, VkPipelineLayout& ShaderLayout, std::shared_ptr<Camera> CameraView)
{
   meshManager->Draw(commandBuffer, ShaderLayout, CameraView);
}

void AssetManager::GUIDraw(VkCommandBuffer& commandBuffer, VkPipelineLayout layout)
{
   ObjManager->GUIDraw(commandBuffer, layout);
}

void AssetManager::Delete()
{
    SceneData->Destroy();
    SkyUniformBuffer->Destroy();

    TextureManagerPtr::GetTextureManagerPtr()->Destory();
    MaterialManagerPtr::GetMaterialManagerPtr()->Destory();
    MeshManagerPtr::GetMeshManagerPtr()->Destroy();
    LightManagerPtr::GetLightManagerPtr()->Destory();
    GuiManagerPtr::GetGuiManagerPtr()->Destory();
    ObjManagerPtr::GetObjManagerPtr()->Destory();
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
