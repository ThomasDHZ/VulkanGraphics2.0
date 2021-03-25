#include "AssetManager.h"

AssetManager::AssetManager()
{
}

AssetManager::AssetManager(VulkanEngine& engine)
{
    textureManager = TextureManager(engine);
    materialManager = MaterialManager(engine, textureManager);
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
    modelManager.ModelList.emplace_back(std::make_shared<Model>(Model(engine, meshManager, textureManager, VertexList, IndexList)));
}

void AssetManager::AddModel(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, uint32_t materialID)
{
    modelManager.ModelList.emplace_back(std::make_shared<Model>(Model(engine, meshManager, VertexList, IndexList, materialID)));
}

void AssetManager::Update(VulkanEngine& engine)
{
    materialManager.UpdateBufferIndex(engine);

    meshManager.Update(engine, materialManager);
    meshManager.UpdateBufferIndex(engine);

    modelManager.Update(engine, materialManager);
}

void AssetManager::Draw(VkCommandBuffer commandBuffer, std::shared_ptr<GraphicsPipeline> pipeline)
{
   meshManager.Draw(commandBuffer, pipeline);
}

void AssetManager::Delete(VulkanEngine& engine)
{
    meshManager.Destroy(engine);
    textureManager.Destory(engine);
    materialManager.Destory(engine);
}
