#include "MaterialManager.h"

MaterialManager::MaterialManager()
{
}

MaterialManager::MaterialManager(std::shared_ptr<VulkanEngine> Engine, std::shared_ptr<TextureManager> texturemanager)
{
	engine = Engine;
	textureManager = texturemanager;
	std::shared_ptr<Material> materialData = std::make_shared<Material>(engine, textureManager);
	LoadMaterial("Default Material", materialData);
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


std::shared_ptr<Material> MaterialManager::LoadMaterial(std::string MaterialName, std::shared_ptr<Material> material)
{
	uint32_t MaterialID = engine->GenerateID();
	MaterialList.emplace_back(material);
	MaterialList.back()->MaterialID = MaterialID;
	MaterialList.back()->MaterialBufferIndex = MaterialList.size();
	MaterialList.back()->UpdateBufferIndexs(engine);

	return material;
}

std::shared_ptr<Material> MaterialManager::LoadMaterial(std::string MaterialName, MaterialTexture& material)
{
	std::shared_ptr<Material> materialData = std::make_shared<Material>(Material(engine, material));

	uint32_t MaterialID = engine->GenerateID();
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
	if (MaterialList.size() == 0)
	{
		VkDescriptorBufferInfo nullBuffer;
		nullBuffer.buffer = VK_NULL_HANDLE;
		nullBuffer.offset = 0;
		nullBuffer.range = VK_WHOLE_SIZE;
		MaterialBufferList.emplace_back(nullBuffer);
	}
	else
	{
		for (int x = 0; x < MaterialList.size(); x++)
		{
			VkDescriptorBufferInfo MaterialBufferInfo = {};
			MaterialBufferInfo.buffer = MaterialList[x]->GetMaterialBuffer();
			MaterialBufferInfo.offset = 0;
			MaterialBufferInfo.range = VK_WHOLE_SIZE;
			MaterialBufferList.emplace_back(MaterialBufferInfo);
		}
	}

	return MaterialBufferList;
}

uint32_t MaterialManager::GetMaterialDescriptorCount()
{
	if (MaterialList.size() > 0)
	{
		return MaterialList.size();
	}
	else
	{
		return 1;
	}
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

void MaterialManager::Update()
{
	for(int x = 0; x < MaterialList.size(); x++)
	{
		MaterialList[x]->MaterialBufferIndex = x;
		MaterialList[x]->UpdateBufferIndexs(engine);
	}
}

void MaterialManager::DeleteMaterial(uint32_t DeleteMaterialBufferIndex)
{
	auto material = GetMaterial(DeleteMaterialBufferIndex);
	material->Delete(engine);
	MaterialList.erase(MaterialList.begin() + DeleteMaterialBufferIndex);

	Update();
}

void MaterialManager::Destory()
{
	for (auto& material : MaterialList)
	{
		material->Delete(engine);
	}
}
