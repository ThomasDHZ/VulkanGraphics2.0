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
	DirectionalLight(std::shared_ptr<VulkanEngine> engine);
	DirectionalLight(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<CameraManager> cameraManager, DirectionalLightBuffer light);
	~DirectionalLight();

	void Update(std::shared_ptr<VulkanEngine> engine) override;
	void Destroy(std::shared_ptr<VulkanEngine> engine) override;
};

