#pragma once
#include <vector>
#include "VulkanEngine.h"
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
	LightManager(VulkanEngine& engine, CameraManager& cameraManager);
	~LightManager();

	void Update(VulkanEngine& engine);
	void UpdateImGui();


	void AddDirectionalLight(VulkanEngine& engine, std::shared_ptr<DirectionalLight> light);
	void AddDirectionalLight(VulkanEngine& engine, CameraManager& cameraManager, DirectionalLightBuffer light);
	void AddPointLight(VulkanEngine& engine, std::shared_ptr<PointLight> light);
	void AddPointLight(VulkanEngine& engine, PointLightBuffer light);
	void AddSpotLight(VulkanEngine& engine, std::shared_ptr<SpotLight> light);
	void AddSpotLight(VulkanEngine& engine, SpotLightBuffer light);
	void DeleteDirectionalLight(VulkanEngine& engine, uint32_t LightBufferIndex);
	void DeletePointLight(VulkanEngine& engine, uint32_t LightBufferIndex);
	void DeleteSpotLight(VulkanEngine& engine, uint32_t LightBufferIndex);

	std::vector<VkDescriptorBufferInfo> GetDirectionalLightBufferListDescriptor();
	std::vector<VkDescriptorBufferInfo> GetPointLightBufferListDescriptor();
	std::vector<VkDescriptorBufferInfo> GetSpotLightBufferListDescriptor();
	void Destory(VulkanEngine& engine);

	uint32_t GetDirectionalLightDescriptorCount() { return DirectionalLightList.size(); }
	uint32_t GetPointLightDescriptorCount() { return PointLightList.size(); }
	uint32_t GetSpotLightDescriptorCount() { return SpotLightList.size(); }
};

