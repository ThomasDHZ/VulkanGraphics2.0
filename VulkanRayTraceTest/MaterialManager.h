#pragma once
#include "Material.h"

class MaterialManager
{
private:

	uint32_t IsMateralLoaded(std::string name);
public:

	std::vector<std::shared_ptr<Material>> MaterialList;

	MaterialManager();
	MaterialManager(std::shared_ptr<VulkanEngine> engine, TextureManager& textureManager);
	~MaterialManager();

	uint32_t GetMaterialBufferIDByMaterialID(uint32_t MaterialID);
	void Update(std::shared_ptr<VulkanEngine> engine);
	void DeleteMaterial(std::shared_ptr<VulkanEngine> engine, uint32_t DeleteMaterialBufferIndex);
	void Destory(std::shared_ptr<VulkanEngine> engine);

	std::shared_ptr<Material> LoadMaterial(std::shared_ptr<VulkanEngine> engine, std::string MaterialName, std::shared_ptr<Material> material);
	std::shared_ptr<Material> LoadMaterial(std::shared_ptr<VulkanEngine> engine, std::string MaterialName, MaterialTexture& material);
	std::shared_ptr<Material> GetDefaultMaterial();
	std::shared_ptr<Material> GetMaterial(uint32_t MaterialID);

	std::vector<VkDescriptorBufferInfo> GetMaterialBufferListDescriptor();
	uint32_t GetMaterialDescriptorCount();
};

