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

	void Update(VulkanEngine& engine);
	void UpdateMaterialIndex(VulkanEngine& engine);
	void DeleteMaterial(VulkanEngine& engine, std::shared_ptr<Material> material);
	void Destory(VulkanEngine& engine);

	uint32_t LoadMaterial(VulkanEngine& engine, std::string MaterialName, std::shared_ptr<Material> material);
	std::shared_ptr<Material> GetMaterial(uint32_t MaterialID);
	std::vector<VkDescriptorBufferInfo> GetMaterialBufferListDescriptor();
};

