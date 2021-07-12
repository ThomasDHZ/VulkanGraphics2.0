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


std::shared_ptr<Material> MaterialManager::LoadMaterial(VulkanEngine& engine, std::string MaterialName, std::shared_ptr<Material> material)
{
	uint32_t MaterialID = engine.GenerateID();
	MaterialList.emplace_back(material);
	MaterialList.back()->MaterialID = MaterialID;
	MaterialList.back()->MaterialBufferIndex = MaterialList.size();
	MaterialList.back()->UpdateBufferIndexs(engine);

	return material;
}

std::shared_ptr<Material> MaterialManager::LoadMaterial(VulkanEngine& engine, std::string MaterialName, MaterialTexture& material)
{
	std::shared_ptr<Material> materialData = std::make_shared<Material>(Material(engine, material));

	uint32_t MaterialID = engine.GenerateID();
	MaterialList.emplace_back(materialData);
	MaterialList.back()->MaterialID = MaterialID;
	MaterialList.back()->MaterialBufferIndex = MaterialList.size();
	MaterialList.back()->UpdateBufferIndexs(engine);

	return MaterialList.back();
}

std::shared_ptr<Material> MaterialManager::GetDefaultMaterial()
{
	auto returnMaterial = MaterialList[0];
	for (auto& material : MaterialList)
	{
		if (material->MaterialID == 0)
		{
			returnMaterial = material;
		}
	}
	return returnMaterial;
}

std::shared_ptr<Material> MaterialManager::GetMaterial(uint32_t MaterialID)
{
	auto returnMaterial = GetDefaultMaterial();
	for (auto& material : MaterialList)
	{
		if (material->MaterialID == MaterialID)
		{
			returnMaterial = material;
		}
	}
	return returnMaterial;
}

std::vector<VkDescriptorBufferInfo> MaterialManager::GetMaterialBufferListDescriptor()
{
	std::vector<VkDescriptorBufferInfo> MaterialBufferList{};
	for (int x = 0; x < MaterialList.size(); x++)
	{
		VkDescriptorBufferInfo MaterialBufferInfo = {};
		MaterialBufferInfo.buffer = MaterialList[x]->GetMaterialBuffer();
		MaterialBufferInfo.offset = 0;
		MaterialBufferInfo.range = VK_WHOLE_SIZE;
		MaterialBufferList.emplace_back(MaterialBufferInfo);
	}

	return MaterialBufferList;
}

uint32_t MaterialManager::GetMaterialBufferIDByMaterialID(uint32_t MaterialID)
{
	uint32_t BufferIndex = 0;
	for (auto& material : MaterialList)
	{
		if (MaterialID == material->MaterialID)
		{
			BufferIndex = material->MaterialBufferIndex;
		}
	}

	return BufferIndex;
}

void MaterialManager::Update(VulkanEngine& engine)
{
	for(int x = 0; x < MaterialList.size(); x++)
	{
		MaterialList[x]->MaterialBufferIndex = x;
		MaterialList[x]->UpdateBufferIndexs(engine);
	}
}

void MaterialManager::DeleteMaterial(VulkanEngine& engine, uint32_t DeleteMaterialBufferIndex)
{
	auto material = GetMaterial(DeleteMaterialBufferIndex);
	material->Delete(engine);
	MaterialList.erase(MaterialList.begin() + DeleteMaterialBufferIndex);

	Update(engine);
}

void MaterialManager::Destory(VulkanEngine& engine)
{
	for (auto& material : MaterialList)
	{
		material->Delete(engine);
	}
}
