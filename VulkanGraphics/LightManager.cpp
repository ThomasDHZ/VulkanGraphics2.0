#include "LightManager.h"

std::shared_ptr<LightManager> LightManagerPtr::lightManager = nullptr;

LightManager::LightManager()
{
}

LightManager::LightManager(std::shared_ptr<VulkanEngine> Engine, std::shared_ptr<CameraManager> cameraManager)
{
	engine = Engine;

	DirectionalLightBuffer dlight = DirectionalLightBuffer();
	dlight.direction = glm::vec4(1.0f);
	dlight.ambient = glm::vec4(0.2f);
	dlight.diffuse = glm::vec4(0.5f);
	dlight.specular = glm::vec4(1.0f);
	AddDirectionalLight(cameraManager, dlight);
	AddDirectionalLight(cameraManager, dlight);

	PointLightBuffer plight = PointLightBuffer();
	plight.position = glm::vec4(0.5f, 1.0f, 0.3f, 1.0f);
	plight.ambient = glm::vec4(0.2f);
	plight.diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 0.0f);
	plight.specular = glm::vec4(1.0f);

	AddPointLight(plight);
	AddPointLight(plight);
	AddSpotLight(SpotLightBuffer());
}

LightManager::~LightManager()
{
}

void LightManager::Update()
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

void LightManager::AddDirectionalLight(std::shared_ptr<DirectionalLight> light)
{
	DirectionalLightList.emplace_back(light);
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}

void LightManager::AddDirectionalLight(std::shared_ptr<CameraManager> cameraManager, DirectionalLightBuffer light)
{
	DirectionalLightList.emplace_back(std::make_shared<DirectionalLight>(DirectionalLight(engine, cameraManager, light)));
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}

void LightManager::AddPointLight(std::shared_ptr<PointLight> light)
{
	PointLightList.emplace_back(light);
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}

void LightManager::AddPointLight(PointLightBuffer light)
{
	PointLightList.emplace_back(std::make_shared<PointLight>(PointLight(engine, light)));
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}

void LightManager::AddSpotLight(std::shared_ptr<SpotLight> light)
{
	SpotLightList.emplace_back(light);
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}

void LightManager::AddSpotLight(SpotLightBuffer light)
{
	SpotLightList.emplace_back(std::make_shared<SpotLight>(SpotLight(engine, light)));
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}

void LightManager::DeleteDirectionalLight(uint32_t LightBufferIndex)
{
	DirectionalLightList[LightBufferIndex]->Destroy(engine);
	DirectionalLightList.erase(DirectionalLightList.begin() + LightBufferIndex);
	Update();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}

void LightManager::DeletePointLight(uint32_t LightBufferIndex)
{
	PointLightList[LightBufferIndex]->Destroy(engine);
	PointLightList.erase(PointLightList.begin() + LightBufferIndex);
	Update();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}

void LightManager::DeleteSpotLight(uint32_t LightBufferIndex)
{
	SpotLightList[LightBufferIndex]->Destroy(engine);
	SpotLightList.erase(SpotLightList.begin() + LightBufferIndex);
	Update();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
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

void LightManager::Destory()
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