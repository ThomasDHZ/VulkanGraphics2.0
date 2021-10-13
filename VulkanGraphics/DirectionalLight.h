#pragma once
#include "Light.h"
#include "LightViewCamera.h"
#include "CameraManager.h"
#include "OrthographicLightViewCamera.h"


class DirectionalLight : public Light<DirectionalLightBuffer>
{
private:
	std::vector<OrthographicLightViewCamera> lightViewCamera;

public:
	DirectionalLight();
	DirectionalLight(std::shared_ptr<VulkanEngine> engine);
	DirectionalLight(DirectionalLightBuffer light);
	~DirectionalLight();

	void Update() override;
	void Destroy() override;
};

