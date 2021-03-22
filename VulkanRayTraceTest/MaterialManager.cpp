#include "MaterialManager.h"

MaterialManager::MaterialManager()
{
}

MaterialManager::MaterialManager(VulkanEngine& engine, TextureManager& textureManager)
{
	std::shared_ptr<Material> materialData = std::make_shared<Material>(engine, textureManager);
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
		if (material->MaterialName == name)
		{
			MaterialID = material->MaterialID;
			return MaterialID;
		}
	}

	return MaterialID;
}


uint32_t MaterialManager::LoadMaterial(VulkanEngine& engine, std::string MaterialName, std::shared_ptr<Material> material)
{
	uint32_t MaterialID = IsMateralLoaded(MaterialName);

		MaterialList.emplace_back(material);
		MaterialList.back()->MaterialID = MaterialID;
		MaterialList.back()->MaterialBufferIndex = MaterialList.size();
		MaterialList.back()->UpdateBufferIndexs(engine);
	
	return MaterialID;
}

std::shared_ptr<Material> MaterialManager::GetMaterial(uint32_t MaterialID)
{
	return MaterialList[MaterialID];
}

std::vector<VkDescriptorBufferInfo> MaterialManager::GetMaterialBufferListDescriptor()
{
	std::vector<VkDescriptorBufferInfo> MaterialBufferList{};
	for (int x = 0; x < MaterialList.size(); x++)
	{
		VkDescriptorBufferInfo MaterialBufferInfo = {};
		MaterialBufferInfo.buffer = MaterialList[x]->MaterialBuffer.Buffer;
		MaterialBufferInfo.offset = 0;
		MaterialBufferInfo.range = VK_WHOLE_SIZE;
		MaterialBufferList.emplace_back(MaterialBufferInfo);
	}

	return MaterialBufferList;
}

void MaterialManager::Update(VulkanEngine& engine)
{
}

void MaterialManager::UpdateMaterialIndex(VulkanEngine& engine)
{
	for(int x = 0; x < MaterialList.size(); x++)
	{
		MaterialList[x]->MaterialBufferIndex = x;
		MaterialList[x]->UpdateBufferIndexs(engine);
	}
}

void MaterialManager::DeleteMaterial(VulkanEngine& engine, std::shared_ptr<Material> material)
{
	const int index = material->MaterialBufferIndex - 1;
	for (int x = 0; x < MaterialList.size(); x++)
	{
		if (material->MaterialBufferIndex < MaterialList[x]->MaterialBufferIndex)
		{
			MaterialList[x]->MaterialBufferIndex -= 1;
		}
	}
	
	material->Delete(engine);
	MaterialList.erase(MaterialList.begin() + index);

	UpdateMaterialIndex(engine);
}

void MaterialManager::Destory(VulkanEngine& engine)
{
	for (auto& material : MaterialList)
	{
		material->Delete(engine);
	}
}
