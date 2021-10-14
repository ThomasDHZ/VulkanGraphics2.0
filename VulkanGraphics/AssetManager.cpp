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

    SceneData->UniformDataInfo.lightSpaceMatrix = lightManager->PointLightList[0]->GetLightSpaceMatrix();
    SceneData->UniformDataInfo.DirectionalLightCount = lightManager->GetDirectionalLightCount();
    SceneData->UniformDataInfo.PointLightCount = lightManager->GetPointLightCount();
    SceneData->UniformDataInfo.SpotLightCount = lightManager->GetSpotLightCount();
    SceneData->UniformDataInfo.SphereAreaLightCount = lightManager->GetSphereAreaLightCount();
    SceneData->UniformDataInfo.TubeAreaLightCount = lightManager->GetTubeAreaLightCount();
    SceneData->UniformDataInfo.RectangleAreaLightCount = lightManager->GetRectangleAreaLightCount();
    SceneData->UniformDataInfo.timer = timer;
    SceneData->Update();
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

    TextureManagerPtr::GetTextureManagerPtr()->Destory();
    MaterialManagerPtr::GetMaterialManagerPtr()->Destory();
    MeshManagerPtr::GetMeshManagerPtr()->Destroy();
    LightManagerPtr::GetLightManagerPtr()->Destory();
    GuiManagerPtr::GetGuiManagerPtr()->Destory();
    ObjManagerPtr::GetObjManagerPtr()->Destory();
}

std::vector<std::shared_ptr<Mesh>> AssetManager::GetMeshByType(MeshTypeFlag type)
{
    return meshManager->GetMeshByType(type);
}
