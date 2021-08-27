#include "MaterialManager.h"

std::shared_ptr<MaterialManager> MaterialManagerPtr::materialManager = nullptr;

MaterialManager::MaterialManager()
{
}

MaterialManager::MaterialManager(std::shared_ptr<VulkanEngine> Engine)
{
	std::shared_ptr<Material> materialData = std::make_shared<Material>(Material("DefaultMaterial"));
	LoadMaterial("Default Material", materialData);
}

MaterialManager::~MaterialManager()
{
}

std::shared_ptr<Material> MaterialManager::LoadMaterial(std::string MaterialName, std::shared_ptr<Material> material)
{
	MaterialList.emplace_back(material);
	UpdateBufferIndex();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;

	return material;
}

std::shared_ptr<Material> MaterialManager::LoadMaterial(std::string MaterialName, MaterialTexture& material)
{
	MaterialList.emplace_back(std::make_shared<Material>(Material(MaterialName, material)));
	UpdateBufferIndex();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;

	return MaterialList.back();
}

std::shared_ptr<Material> MaterialManager::GetDefaultMaterial()
{
	auto returnMaterial = MaterialList[0];
	for (auto& material : MaterialList)
	{
		if (material->GetMaterialID() == 0)
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
		if (material->GetMaterialID() == MaterialID)
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
		if (MaterialID == material->GetMaterialID())
		{
			BufferIndex = material->GetMaterialBufferIndex();
		}
	}

	return BufferIndex;
}

void MaterialManager::UpdateBufferIndex()
{
	for(int x = 0; x < MaterialList.size(); x++)
	{
		MaterialList[x]->SetMaterialBufferIndex(x);
		MaterialList[x]->UpdateTextureIndexs();
	}
}

void MaterialManager::DeleteMaterial(uint32_t DeleteMaterialBufferIndex)
{
	auto material = GetMaterial(DeleteMaterialBufferIndex);
	material->Delete();
	MaterialList.erase(MaterialList.begin() + DeleteMaterialBufferIndex);
	UpdateBufferIndex();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}

void MaterialManager::Destory()
{
	for (auto& material : MaterialList)
	{
		material->Delete();
	}
}
