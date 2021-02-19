#pragma once
#include "VulkanEngine.h"
#include "Model.h"

class ModelRenderManager
{
private:
	std::vector<Model> ModelList;
	std::vector<Material> MaterialList;

	std::vector<VulkanBuffer> VertexBufferList;
	std::vector<VulkanBuffer> IndexBufferList;

	std::vector<VkDescriptorBufferInfo> VertexBufferListDescriptor();
	std::vector<VkDescriptorBufferInfo> IndexBufferListDescriptor();
	std::vector<VkDescriptorBufferInfo> MaterialBufferListDescriptor();
	std::vector<VkDescriptorBufferInfo> TextureBufferListDescriptor();
public:
	ModelRenderManager();
	ModelRenderManager(VulkanEngine& engine);
	~ModelRenderManager();

	void AddModel(VulkanEngine& engine, TextureManager& textureManager, const std::string& FilePath);
	void UpdateMeshDescriptors(VulkanEngine& engine, VkDescriptorSet& descriptorSet);
};

