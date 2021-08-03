#include "LightManager.h"

LightManager::LightManager()
{
}

LightManager::LightManager(std::shared_ptr<VulkanEngine> engine, CameraManager& cameraManager)
{
	DirectionalLightBuffer dlight = DirectionalLightBuffer();
	dlight.direction = glm::vec4(1.0f);
	dlight.ambient = glm::vec4(0.2f);
	dlight.diffuse = glm::vec4(0.5f);
	dlight.specular = glm::vec4(1.0f);
	AddDirectionalLight(engine, cameraManager, dlight);
	AddDirectionalLight(engine, cameraManager, dlight);

	PointLightBuffer plight = PointLightBuffer();
	plight.position = glm::vec4(0.5f, 1.0f, 0.3f, 1.0f);
	plight.ambient = glm::vec4(0.2f);
	plight.diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 0.0f);
	plight.specular = glm::vec4(1.0f);

	AddPointLight(engine, plight);
	AddPointLight(engine, plight);
	AddSpotLight(engine, SpotLightBuffer());
}

LightManager::~LightManager()
{
}

void LightManager::Update(std::shared_ptr<VulkanEngine> engine)
{
	for (auto& directionalLight : DirectionalLightList)
	{
		directionalLight->Update(engine);
	}
	for (auto& pointLight : PointLightList)
	{
		pointLight->Update(engine);
	}
	for (auto& spotLight : SpotLightList)
	{
		spotLight->Update(engine);
	}
}

void LightManager::UpdateImGui()
{
}

void LightManager::AddDirectionalLight(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<DirectionalLight> light)
{
	DirectionalLightList.emplace_back(light);
}

void LightManager::AddDirectionalLight(std::shared_ptr<VulkanEngine> engine, CameraManager& cameraManager, DirectionalLightBuffer light)
{
	DirectionalLightList.emplace_back(std::make_shared<DirectionalLight>(DirectionalLight(engine, cameraManager, light)));
}

void LightManager::AddPointLight(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<PointLight> light)
{
	PointLightList.emplace_back(light);
}

void LightManager::AddPointLight(std::shared_ptr<VulkanEngine> engine, PointLightBuffer light)
{
	PointLightList.emplace_back(std::make_shared<PointLight>(PointLight(engine, light)));
}

void LightManager::AddSpotLight(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<SpotLight> light)
{
	SpotLightList.emplace_back(light);
}

void LightManager::AddSpotLight(std::shared_ptr<VulkanEngine> engine, SpotLightBuffer light)
{
	SpotLightList.emplace_back(std::make_shared<SpotLight>(SpotLight(engine, light)));
}

void LightManager::DeleteDirectionalLight(std::shared_ptr<VulkanEngine> engine, uint32_t LightBufferIndex)
{
	DirectionalLightList[LightBufferIndex]->Destroy(engine);
	DirectionalLightList.erase(DirectionalLightList.begin() + LightBufferIndex);
	Update(engine);
}

void LightManager::DeletePointLight(std::shared_ptr<VulkanEngine> engine, uint32_t LightBufferIndex)
{
	PointLightList[LightBufferIndex]->Destroy(engine);
	PointLightList.erase(PointLightList.begin() + LightBufferIndex);
	Update(engine);
}

void LightManager::DeleteSpotLight(std::shared_ptr<VulkanEngine> engine, uint32_t LightBufferIndex)
{
	SpotLightList[LightBufferIndex]->Destroy(engine);

	SpotLightList[LightBufferIndex]->Destroy(engine);
	SpotLightList.erase(SpotLightList.begin() + LightBufferIndex);
	Update(engine);
}

std::vector<VkDescriptorBufferInfo> LightManager::GetDirectionalLightBufferListDescriptor()
{
	std::vector<VkDescriptorBufferInfo> DirectionalLightBufferList{};
	if (DirectionalLightList.size() == 0)
	{
		VkDescriptorBufferInfo nullBuffer;
		nullBuffer.buffer = VK_NULL_HANDLE;
		nullBuffer.offset = 0;
		nullBuffer.range = VK_WHOLE_SIZE;
		DirectionalLightBufferList.emplace_back(nullBuffer);
	}
	else
	{
		for (int x = 0; x < DirectionalLightList.size(); x++)
		{
			VkDescriptorBufferInfo DirectionalLightBufferInfo = {};
			DirectionalLightBufferInfo.buffer = DirectionalLightList[x]->GetLightBuffer();
			DirectionalLightBufferInfo.offset = 0;
			DirectionalLightBufferInfo.range = VK_WHOLE_SIZE;
			DirectionalLightBufferList.emplace_back(DirectionalLightBufferInfo);
		}
	}

	return DirectionalLightBufferList;
}

std::vector<VkDescriptorBufferInfo> LightManager::GetPointLightBufferListDescriptor()
{
	std::vector<VkDescriptorBufferInfo> PointLightBufferList{};
	if (PointLightList.size() == 0)
	{
		VkDescriptorBufferInfo nullBuffer;
		nullBuffer.buffer = VK_NULL_HANDLE;
		nullBuffer.offset = 0;
		nullBuffer.range = VK_WHOLE_SIZE;
		PointLightBufferList.emplace_back(nullBuffer);
	}
	else
	{
		for (int x = 0; x < PointLightList.size(); x++)
		{
			VkDescriptorBufferInfo PointLightBufferInfo = {};
			PointLightBufferInfo.buffer = PointLightList[x]->GetLightBuffer();
			PointLightBufferInfo.offset = 0;
			PointLightBufferInfo.range = VK_WHOLE_SIZE;
			PointLightBufferList.emplace_back(PointLightBufferInfo);
		}
	}

	return PointLightBufferList;
}

std::vector<VkDescriptorBufferInfo> LightManager::GetSpotLightBufferListDescriptor()
{
	std::vector<VkDescriptorBufferInfo> SpotLightBufferList{};
	if (SpotLightList.size() == 0)
	{
		VkDescriptorBufferInfo nullBuffer;
		nullBuffer.buffer = VK_NULL_HANDLE;
		nullBuffer.offset = 0;
		nullBuffer.range = VK_WHOLE_SIZE;
		SpotLightBufferList.emplace_back(nullBuffer);
	}
	else
	{
		for (int x = 0; x < SpotLightList.size(); x++)
		{
			VkDescriptorBufferInfo SpotLightBufferInfo = {};
			SpotLightBufferInfo.buffer = SpotLightList[x]->GetLightBuffer();
			SpotLightBufferInfo.offset = 0;
			SpotLightBufferInfo.range = VK_WHOLE_SIZE;
			SpotLightBufferList.emplace_back(SpotLightBufferInfo);
		}
	}

	return SpotLightBufferList;
}

void LightManager::Destory(std::shared_ptr<VulkanEngine> engine)
{
	for (auto& directionalLight : DirectionalLightList)
	{
		directionalLight->Destroy(engine);
	}
	for (auto& pointLight : PointLightList)
	{
		pointLight->Destroy(engine);
	}
	for (auto& spotLight : SpotLightList)
	{
		spotLight->Destroy(engine);
	}
}
uint32_t LightManager::GetDirectionalLightDescriptorCount() 
{
	if (DirectionalLightList.size() > 0)
	{
		return DirectionalLightList.size();
	}
	else
	{
		return 1;
	}
}

uint32_t LightManager::GetPointLightDescriptorCount() 
{
	if (PointLightList.size() > 0)
	{
		return PointLightList.size();
	}
	else
	{
		return 1;
	}
}

uint32_t LightManager::GetSpotLightDescriptorCount() 
{
	if (SpotLightList.size() > 0)
	{
		return SpotLightList.size();
	}
	else
	{
		return 1;
	}
}