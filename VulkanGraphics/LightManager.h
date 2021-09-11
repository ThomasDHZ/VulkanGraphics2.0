#pragma once
#include <vector>
#include "Vulkanengine.h"
#include "CameraManager.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "SphereAreaLight.h"
#include "TubeAreaLight.h"
#include "RectangleAreaLight.h"

class LightManager
{
	friend class DirectionalLight;
	friend class PointLight;
	friend class SpotLight;
	friend class SphereAreaLight;
	friend class TubeAreaLight;
	friend class RectangleAreaLight;

private:
	std::shared_ptr<VulkanEngine> engine;
public:
	std::vector<std::shared_ptr<DirectionalLight>> DirectionalLightList;
	std::vector<std::shared_ptr<PointLight>> PointLightList;
	std::vector<std::shared_ptr<SpotLight>> SpotLightList;
	std::vector<std::shared_ptr<SphereAreaLight>> SphereAreaLightList;
	std::vector<std::shared_ptr<TubeAreaLight>> TubeAreaLightList;
	std::vector<std::shared_ptr<RectangleAreaLight>> RectangleAreaLightList;

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
	void AddSphereAreaLight(std::shared_ptr<SphereAreaLight> light);
	void AddSphereAreaLight(SphereAreaLightBuffer light);
	void AddTubeAreaLight(std::shared_ptr<TubeAreaLight> light);
	void AddTubeAreaLight(TubeAreaLightBuffer light);
	void AddRectangleAreaLight(std::shared_ptr<RectangleAreaLight> light);
	void AddRectangleAreaLight(RectangleAreaLightBuffer light);

	void DeleteDirectionalLight(uint32_t LightBufferIndex);
	void DeletePointLight(uint32_t LightBufferIndex);
	void DeleteSpotLight(uint32_t LightBufferIndex);
	void DeleteSphereAreaLight(uint32_t LightBufferIndex);
	void DeleteTubeAreaLight(uint32_t LightBufferIndex);
	void DeleteRectangleAreaLight(uint32_t LightBufferIndex);

	std::vector<VkDescriptorBufferInfo> GetDirectionalLightBufferListDescriptor();
	std::vector<VkDescriptorBufferInfo> GetPointLightBufferListDescriptor();
	std::vector<VkDescriptorBufferInfo> GetSpotLightBufferListDescriptor();
	std::vector<VkDescriptorBufferInfo> GetSphereAreaLightDescriptor();
	std::vector<VkDescriptorBufferInfo> GetTubeAreaLightDescriptor();
	std::vector<VkDescriptorBufferInfo> GetRectangleAreaLightDescriptor();

	uint32_t GetDirectionalLightDescriptorCount();
	uint32_t GetPointLightDescriptorCount();
	uint32_t GetSpotLightDescriptorCount();
	uint32_t GetSphereAreaLightDescriptorCount();
	uint32_t GetTubeAreaLightDescriptorCount();
	uint32_t GetRectangleAreaLightDescriptorCount();

	void Destory();
};

class LightManagerPtr
{
private:
	static std::shared_ptr<LightManager> lightManager;
public:
	static void SetUpPtr(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<CameraManager> camera)
	{
		if (lightManager == nullptr)
		{
			lightManager = std::make_shared<LightManager>(LightManager(engine, camera));
		}
		else
		{
			std::cout << "Light Manager has already been initialized." << std::endl;
		}
	}

	static std::shared_ptr<LightManager> GetLightManagerPtr()
	{
		return lightManager;
	}
};
