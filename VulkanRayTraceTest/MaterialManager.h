#pragma once
#include "Material.h"

class MaterialManager
{
private:


	uint32_t IsMateralLoaded(std::string name);
public:

	std::vector<std::shared_ptr<Material>> MaterialList;

	MaterialManager();
	MaterialManager(VulkanEngine& engine, TextureManager& textureManager);
	~MaterialManager();

	uint32_t GetMaterialBufferIDByMaterialID(uint32_t MaterialID);
	void UpdateBufferIndex(VulkanEngine& engine);
	void DeleteMaterial(VulkanEngine& engine, uint32_t DeleteMaterialBufferIndex);
	void Destory(VulkanEngine& engine);

	uint32_t LoadMaterial(VulkanEngine& engine, std::string MaterialName, std::shared_ptr<Material> material);
	std::shared_ptr<Material> GetMaterial(uint32_t MaterialID);

	std::vector<VkDescriptorBufferInfo> GetMaterialBufferListDescriptor();
	uint32_t GetMaterialDescriptorCount() { return MaterialList.size(); }
};

