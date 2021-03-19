#pragma once
#include "Material.h"



class MaterialManager
{
private:
	std::vector<Material> MaterialList;

	uint32_t IsMateralLoaded(std::string name);
public:

	MaterialManager();
	MaterialManager(VulkanEngine& engine);
	~MaterialManager();

	void Update(VulkanEngine& engine);
	void Destory(VulkanEngine& engine);

	uint32_t LoadMaterial(VulkanEngine& engine, std::string MaterialName, MaterialData& materialData);
	Material GetMaterial(uint32_t MaterialID);
	std::vector<VkDescriptorBufferInfo> GetMaterialBufferListDescriptor();
};

