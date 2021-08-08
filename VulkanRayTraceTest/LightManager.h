#pragma once
#include <vector>
#include "Vulkanengine.h"
#include "CameraManager.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

class LightManager
{
	friend class DirectionalLight;
	friend class PointLight;
	friend class SpotLight;
private:
	std::shared_ptr<VulkanEngine> engine;
public:
	std::vector<std::shared_ptr<DirectionalLight>> DirectionalLightList;
	std::vector<std::shared_ptr<PointLight>> PointLightList;
	std::vector<std::shared_ptr<SpotLight>> SpotLightList;

	LightManager();
	LightManager(std::shared_ptr<VulkanEngine> Engine, std::shared_ptr<CameraManager> cameraManager);
	~LightManager();

	void Update();
	void UpdateImGui();


	void AddDirectionalLight(std::shared_ptr<DirectionalLight> light);
	void AddDirectionalLight(std::shared_ptr<CameraManager> cameraManager, DirectionalLightBuffer light);
	void AddPointLight(std::shared_ptr<PointLight> light);
	void AddPointLight(PointLightBuffer light);
	void AddSpotLight(std::shared_ptr<SpotLight> light);
	void AddSpotLight(SpotLightBuffer light);
	void DeleteDirectionalLight(uint32_t LightBufferIndex);
	void DeletePointLight(uint32_t LightBufferIndex);
	void DeleteSpotLight(uint32_t LightBufferIndex);

	std::vector<VkDescriptorBufferInfo> GetDirectionalLightBufferListDescriptor();
	std::vector<VkDescriptorBufferInfo> GetPointLightBufferListDescriptor();
	std::vector<VkDescriptorBufferInfo> GetSpotLightBufferListDescriptor();
	void Destory();

	uint32_t GetDirectionalLightDescriptorCount();
	uint32_t GetPointLightDescriptorCount();
	uint32_t GetSpotLightDescriptorCount();
};

class LightManagerPtr
{
private:
	static std::shared_ptr<LightManager> lightManager;
public:
	static void SetUpPtr(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<CameraManager> camera)
	{
		lightManager = std::make_shared<LightManager>(LightManager(engine, camera));
	}

	static std::shared_ptr<LightManager> GetLightManagerPtr()
	{
		return lightManager;
	}
};
