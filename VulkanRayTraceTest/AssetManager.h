#pragma once
#include "VulkanEngine.h"
#include "Model.h"
#include "MaterialManager.h"
#include "TextureManager.h"
#include "MeshManager.h"
#include "ModelManager.h"
class AssetManager
{
	friend class MaterialManager;
	friend class TextureManager;
	friend class MeshManager;
	friend class ModelManager;
private:

public:
	MaterialManager materialManager;
	TextureManager textureManager;
	MeshManager meshManager;
	ModelManager modelManager;

	AssetManager();
	AssetManager(VulkanEngine& engine);
	~AssetManager();

	void AddModel(VulkanEngine& engine, MaterialManager& materailManager, const std::string& FilePath);
	void AddModel(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList);
	void AddModel(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, std::shared_ptr<Material> material);
	void UpdateMeshDescriptors(VulkanEngine& engine, VkDescriptorSet& descriptorSet);

	void Update(VulkanEngine& engine);

	std::vector<VkDescriptorBufferInfo> GetVertexBufferListDescriptor();
	std::vector<VkDescriptorBufferInfo> GetIndexBufferListDescriptor();
	std::vector<VkDescriptorBufferInfo> GetMeshDataListDescriptor();
	std::vector<VkDescriptorBufferInfo> GetTransformBufferListDescriptor();
	std::vector<VkDescriptorBufferInfo> GetTransformInverseBufferListDescriptor();
	std::vector<VkDescriptorBufferInfo> GetMaterialBufferListDescriptor(std::vector<std::shared_ptr<Material>> MaterialList);
	std::vector<VkDescriptorImageInfo>  GetTextureBufferListDescriptor();
	std::vector<VkDescriptorImageInfo>  Get3DTextureBufferListDescriptor();

	uint32_t GetMeshPropertiesBufferListDescriptorCount();
	uint32_t GetVertexBufferListDescriptorCount();
	uint32_t GetIndexBufferListDescriptorCount();
	uint32_t GetTransformBufferListDescriptorCount();
	uint32_t GetMaterialBufferListDescriptorCount(std::vector<std::shared_ptr<Material>> MaterialList);
	uint32_t GetTextureBufferListDescriptorCount();
};

