#pragma once
#include "Material.h"

class MaterialManager
{
private:
	std::vector<Material> MaterialList;

	uint32_t IsMateralLoaded(std::string name);
public:

	MaterialManager();
	MaterialManager(VulkanEngine& engine, TextureManager& textureManager);
	~MaterialManager();

	void Update(VulkanEngine& engine);
	void Destory(VulkanEngine& engine);

	uint32_t LoadMaterial(VulkanEngine& engine, std::string MaterialName, Material& material);
	Material GetMaterial(uint32_t MaterialID);
	std::vector<VkDescriptorBufferInfo> GetMaterialBufferListDescriptor();
};

