#include "MaterialManager.h"

MaterialManager::MaterialManager()
{
}

MaterialManager::MaterialManager(VulkanEngine& engine)
{
	MaterialData materialData;
	LoadMaterial(engine, "Default Material", materialData);
}

MaterialManager::~MaterialManager()
{
}

uint32_t MaterialManager::IsMateralLoaded(std::string name)
{
	uint32_t MaterialID = -1;
	for (auto material : MaterialList)
	{
		if (material.MaterialName == name)
		{
			MaterialID = material.MaterialID;
			return MaterialID;
		}
	}

	return MaterialID;
}


uint32_t MaterialManager::LoadMaterial(VulkanEngine& engine, std::string MaterialName, MaterialData& materialData)
{
	
	uint32_t MaterialID = MaterialList.size();

		Material material{};
		material.MaterialName = MaterialName;
		material.MaterialID = MaterialID;
		material.MaterialData = materialData;
		material.MaterialBuffer = VulkanBuffer(engine.Device, engine.PhysicalDevice, sizeof(MaterialData), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &materialData);
		MaterialList.emplace_back(material);
	
	return MaterialID;
}

Material MaterialManager::GetMaterial(uint32_t MaterialID)
{
	return MaterialList[MaterialID];
}

std::vector<VkDescriptorBufferInfo> MaterialManager::GetMaterialBufferListDescriptor()
{

	std::vector<VkDescriptorBufferInfo> MaterialBufferList{};
	for (int x = 0; x < MaterialList.size(); x++)
	{
		VkDescriptorBufferInfo MaterialBufferInfo = {};
		MaterialBufferInfo.buffer = MaterialList[x].MaterialBuffer.Buffer;
		MaterialBufferInfo.offset = 0;
		MaterialBufferInfo.range = VK_WHOLE_SIZE;
		MaterialBufferList.emplace_back(MaterialBufferInfo);
	}

	return MaterialBufferList;
}

void MaterialManager::Update(VulkanEngine& engine)
{
}

void MaterialManager::Destory(VulkanEngine& engine)
{
}
