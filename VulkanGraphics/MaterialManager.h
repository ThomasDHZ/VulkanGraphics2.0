#pragma once
#include "Material.h"

class MaterialManager
{
private:
	std::vector<std::shared_ptr<Material>> MaterialList;

public:
	MaterialManager();
	MaterialManager(std::shared_ptr<VulkanEngine> engine);
	~MaterialManager();

	uint32_t GetMaterialBufferIDByMaterialID(uint32_t MaterialID);
	void UpdateBufferIndex();
	void DeleteMaterial(uint32_t DeleteMaterialBufferIndex);
	void Destory();

	std::shared_ptr<Material> LoadMaterial(std::string MaterialName, std::shared_ptr<Material> material);
	std::shared_ptr<Material> LoadMaterial(std::string MaterialName, MaterialTexture& materialData);
	std::shared_ptr<Material> GetDefaultMaterial();
	std::shared_ptr<Material> GetMaterial(uint32_t MaterialID);

	std::vector<VkDescriptorBufferInfo> GetMaterialBufferListDescriptor();
	uint32_t GetMaterialDescriptorCount();
};

class MaterialManagerPtr
{
private:
	static std::shared_ptr<MaterialManager> materialManager;
public:
	static void SetUpPtr(std::shared_ptr<VulkanEngine> engine)
	{
		if (materialManager == nullptr)
		{
			materialManager = std::make_shared<MaterialManager>(MaterialManager(engine));
		}
		else
		{
			std::cout << "Material Manager has already been initialized." << std::endl;
		}
	}

	static std::shared_ptr<MaterialManager> GetMaterialManagerPtr()
	{
		return materialManager;
	}
};