#pragma once
#include "VulkanEngine.h"
#include "Model.h"

struct MaterialStruct
{
	Material material;
	VulkanBuffer MaterialBuffer;
};

class ModelRenderManager
{
private:
	std::vector<VulkanBuffer> VertexBufferList;
	std::vector<VulkanBuffer> IndexBufferList;

public:
	std::vector<Model> ModelList;
	std::vector<Material> MaterialList;
	TextureManager textureManager;

	ModelRenderManager();
	ModelRenderManager(VulkanEngine& engine);
	~ModelRenderManager();

	void AddModel(VulkanEngine& engine, const std::string& FilePath);
	void AddModel(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList);
	void AddModel(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, Material& material);
	void UpdateMeshDescriptors(VulkanEngine& engine, VkDescriptorSet& descriptorSet);

	std::vector<VkDescriptorBufferInfo> GetVertexBufferListDescriptor();
	std::vector<VkDescriptorBufferInfo> GetIndexBufferListDescriptor();
	std::vector<VkDescriptorBufferInfo> GetTransformBufferListDescriptor();
	std::vector<VkDescriptorBufferInfo> GetTransformInverseBufferListDescriptor();
	std::vector<VkDescriptorBufferInfo> GetMaterialBufferListDescriptor();
	std::vector<VkDescriptorImageInfo>  GetTextureBufferListDescriptor();
	std::vector<VkDescriptorImageInfo>  Get3DTextureBufferListDescriptor();

	uint32_t GetVertexBufferListDescriptorCount();
	uint32_t GetIndexBufferListDescriptorCount();
	uint32_t GetTransformBufferListDescriptorCount();
	uint32_t GetMaterialBufferListDescriptorCount();
	uint32_t GetTextureBufferListDescriptorCount();
};

