#include "AssetManager.h"

AssetManager::AssetManager()
{
}

AssetManager::AssetManager(VulkanEngine& engine, std::shared_ptr<VulkanWindow> window)
{
    inputManager = InputManager(window);
    cameraManager = CameraManager(engine);
    textureManager = TextureManager(engine);
    materialManager = MaterialManager(engine, textureManager);
    lightManager = LightManager(engine, cameraManager);
    guiManager = GUIManager(engine, materialManager, textureManager);
    ObjManager = ObjectManager(engine, materialManager, textureManager);

    SceneData = std::make_shared<SceneDataUniformBuffer>(SceneDataUniformBuffer(engine));
    SkyUniformBuffer = std::make_shared<UniformData<SkyboxUniformBuffer>>(engine);

}

AssetManager::~AssetManager()
{
}

void AssetManager::AddModel()
{
    modelManager.ModelList.emplace_back(std::make_shared<Model>(Model()));
}

void AssetManager::AddModel(VulkanEngine& engine, const std::string& FilePath)
{
    modelManager.ModelList.emplace_back(std::make_shared<Model>(Model(engine, meshManager, materialManager, textureManager, FilePath)));
}

void AssetManager::AddModel(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList)
{
    modelManager.ModelList.emplace_back(std::make_shared<Model>(Model(engine, meshManager, materialManager, textureManager, VertexList, IndexList)));
}

void AssetManager::AddModel(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, std::shared_ptr<Material> material)
{
    modelManager.ModelList.emplace_back(std::make_shared<Model>(Model(engine, meshManager, VertexList, IndexList, material)));
}

void AssetManager::LoadFont(VulkanEngine& engine, const std::string FontLocation)
{
    guiManager.LoadFont(engine, materialManager, textureManager, FontLocation);
}

void AssetManager::Update(VulkanEngine& engine, std::shared_ptr<VulkanWindow> window, bool RayTraceFlag)
{
    float timer = engine.VulkanTimer();
    cameraManager.Update(engine);
    inputManager.Update(cameraManager);
    materialManager.Update(engine);
    textureManager.Update(engine);
    meshManager.Update(engine, inputManager, materialManager, cameraManager.ActiveCamera);
    modelManager.Update(engine, inputManager, materialManager, RayTraceFlag);
    lightManager.Update(engine);
    guiManager.Update(engine, inputManager);
    ObjManager.Update(engine, inputManager);
    if (cameraManager.ActiveCamera->cameraType == CameraType::Perspective_Camera)
    {
       // SceneData->UniformDataInfo.sLight.direction = static_cast<PerspectiveCamera*>(ActiveCamera.get())->GetFront();
    }

    SceneData->UniformDataInfo.DirectionalLightCount = lightManager.GetDirectionalLightDescriptorCount();
    SceneData->UniformDataInfo.PointLightCount = lightManager.GetPointLightDescriptorCount();
    SceneData->UniformDataInfo.SpotLightCount = lightManager.GetSpotLightDescriptorCount();
    SceneData->UniformDataInfo.viewInverse = glm::inverse(cameraManager.ActiveCamera->GetViewMatrix());
    SceneData->UniformDataInfo.projInverse = glm::inverse(cameraManager.ActiveCamera->GetProjectionMatrix());
    SceneData->UniformDataInfo.projInverse[1][1] *= -1;
    SceneData->UniformDataInfo.view = cameraManager.ActiveCamera->GetViewMatrix();
    SceneData->UniformDataInfo.proj = cameraManager.ActiveCamera->GetProjectionMatrix();
    SceneData->UniformDataInfo.proj[1][1] *= -1;
    SceneData->UniformDataInfo.viewPos = glm::vec4(cameraManager.ActiveCamera->GetPosition(), 0.0f);
    SceneData->UniformDataInfo.timer = timer;
    SceneData->Update(engine);

    SkyUniformBuffer->UniformDataInfo.viewInverse = glm::inverse(glm::mat4(glm::mat3(cameraManager.ActiveCamera->GetViewMatrix())));
    SkyUniformBuffer->UniformDataInfo.projInverse = glm::inverse(glm::perspective(glm::radians(cameraManager.ActiveCamera->GetZoom()), engine.SwapChain.GetSwapChainResolution().width / (float)engine.SwapChain.GetSwapChainResolution().height, 0.1f, 100.0f));
    SkyUniformBuffer->UniformDataInfo.projInverse[1][1] *= -1;
    SkyUniformBuffer->UniformDataInfo.view = glm::mat4(glm::mat3(cameraManager.ActiveCamera->GetViewMatrix()));
    SkyUniformBuffer->UniformDataInfo.proj = glm::perspective(glm::radians(cameraManager.ActiveCamera->GetZoom()), engine.SwapChain.GetSwapChainResolution().width / (float)engine.SwapChain.GetSwapChainResolution().height, 0.1f, 100.0f);
    SkyUniformBuffer->UniformDataInfo.proj[1][1] *= -1;
    SkyUniformBuffer->UniformDataInfo.viewPos = glm::vec4(cameraManager.ActiveCamera->GetPosition(), 0.0f);
    SkyUniformBuffer->Update(engine);
}

void AssetManager::Draw(VkCommandBuffer commandBuffer, VkRenderPassBeginInfo renderPassInfo, VkPipelineLayout layout, RenderPassID renderPassID, std::shared_ptr<Camera> CameraView)
{
   meshManager.Draw(commandBuffer, renderPassInfo, layout, renderPassID, CameraView);
}

void AssetManager::GUIDraw(VkCommandBuffer& commandBuffer, VkPipelineLayout layout)
{
   ObjManager.GUIDraw(commandBuffer, layout);
}

void AssetManager::Delete(VulkanEngine& engine)
{
    for (auto& model : modelManager.ModelList)
    {
        model->Destory(engine);
    }

    SceneData->Destroy(engine);
    SkyUniformBuffer->Destroy(engine);

    meshManager.Destroy(engine);
    textureManager.Destory(engine);
    materialManager.Destory(engine);
    lightManager.Destory(engine);
    ObjManager.Destory(engine);
}

std::vector<std::shared_ptr<Mesh>> AssetManager::GetMeshByType(MeshTypeFlag type)
{
    std::vector<std::shared_ptr<Mesh>> ReturnMeshs;
    for (auto mesh : meshManager.MeshList)
    {
        if (mesh->MeshType == type)
        {
           ReturnMeshs.emplace_back(mesh);
        }
    }
    return ReturnMeshs;
}
