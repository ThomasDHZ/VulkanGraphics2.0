#pragma once
#include "VulkanEngine.h"
#include "Model.h"
#include "MaterialManager.h"
#include "TextureManager.h"
#include "MeshManager.h"
#include "ModelManager.h"
class AssetManager
{
private:

public:
	MaterialManager materialManager;
	TextureManager textureManager;
	MeshManager meshManager;
	ModelManager modelManager;

	AssetManager();
	AssetManager(VulkanEngine& engine);
	~AssetManager();

	void AddModel();
	void AddModel(VulkanEngine& engine, const std::string& FilePath);
	void AddModel(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList);
	void AddModel(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, uint32_t materialID);

	void Update(VulkanEngine& engine);
	void Draw(VkCommandBuffer commandBuffer, std::shared_ptr<GraphicsPipeline> pipeline);
	void Delete(VulkanEngine& engine);

	std::vector<VkDescriptorBufferInfo> GetVertexBufferListDescriptors() { return meshManager.GetVertexBufferListDescriptors(); }
	std::vector<VkDescriptorBufferInfo> GetIndexBufferListDescriptors() { return meshManager.GetIndexBufferListDescriptors(); }
	std::vector<VkDescriptorBufferInfo> GetTransformBufferListDescriptors() { return meshManager.GetTransformBufferListDescriptors(); }
	std::vector<VkDescriptorBufferInfo> GetMeshPropertiesListDescriptors() { return meshManager.GetMeshPropertiesListDescriptors(); }
	std::vector<VkDescriptorBufferInfo> GetMaterialBufferListDescriptor() { return materialManager.GetMaterialBufferListDescriptor(); }
	std::vector<VkDescriptorImageInfo>  GetTextureBufferListDescriptor() { return textureManager.GetTextureBufferListDescriptor(); }
	std::vector<VkDescriptorImageInfo>  Get3DTextureBufferListDescriptor() { return textureManager.GetTextureBufferListDescriptor(); }
	VkDescriptorImageInfo  GetSkyBoxTextureBufferListDescriptor() { return textureManager.GetSkyBoxTextureBufferListDescriptor(); }

	uint32_t GetMeshDescriptorCount() { return meshManager.GetMeshDescriptorCount(); }
	uint32_t GetMaterialDescriptorCount() { return materialManager.GetMaterialDescriptorCount(); }
	uint32_t GetTextureBufferDescriptorCount() { return textureManager.GetTextureBufferDescriptorCount(); }
	uint32_t Get3DTextureBufferDescriptorCount() { return textureManager.Get3DTextureBufferDescriptorCount(); }
};

