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

public:
	std::vector<std::shared_ptr<DirectionalLight>> DirectionalLightList;
	std::vector<std::shared_ptr<PointLight>> PointLightList;
	std::vector<std::shared_ptr<SpotLight>> SpotLightList;

	LightManager();
	LightManager(std::shared_ptr<VulkanEngine> engine, CameraManager& cameraManager);
	~LightManager();

	void Update(std::shared_ptr<VulkanEngine> engine);
	void UpdateImGui();


	void AddDirectionalLight(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<DirectionalLight> light);
	void AddDirectionalLight(std::shared_ptr<VulkanEngine> engine, CameraManager& cameraManager, DirectionalLightBuffer light);
	void AddPointLight(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<PointLight> light);
	void AddPointLight(std::shared_ptr<VulkanEngine> engine, PointLightBuffer light);
	void AddSpotLight(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<SpotLight> light);
	void AddSpotLight(std::shared_ptr<VulkanEngine> engine, SpotLightBuffer light);
	void DeleteDirectionalLight(std::shared_ptr<VulkanEngine> engine, uint32_t LightBufferIndex);
	void DeletePointLight(std::shared_ptr<VulkanEngine> engine, uint32_t LightBufferIndex);
	void DeleteSpotLight(std::shared_ptr<VulkanEngine> engine, uint32_t LightBufferIndex);

	std::vector<VkDescriptorBufferInfo> GetDirectionalLightBufferListDescriptor();
	std::vector<VkDescriptorBufferInfo> GetPointLightBufferListDescriptor();
	std::vector<VkDescriptorBufferInfo> GetSpotLightBufferListDescriptor();
	void Destory(std::shared_ptr<VulkanEngine> engine);

	uint32_t GetDirectionalLightDescriptorCount();
	uint32_t GetPointLightDescriptorCount();
	uint32_t GetSpotLightDescriptorCount();
};

