#pragma once
#include "Light.h"
#include "LightViewCamera.h"
#include "CameraManager.h"


class DirectionalLight : public Light<DirectionalLightBuffer>
{
private:
	std::shared_ptr<LightViewCamera> lightViewCamera;
public:
	DirectionalLight();
	DirectionalLight(VulkanEngine& engine);
	DirectionalLight(VulkanEngine& engine, CameraManager& cameraManager, DirectionalLightBuffer light);
	~DirectionalLight();

	void Update(VulkanEngine& engine) override;
	void Destroy(VulkanEngine& engine) override;
};

