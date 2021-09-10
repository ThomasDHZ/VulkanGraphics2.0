#pragma once
#include "Light.h"
#include "LightViewCamera.h"
#include "CameraManager.h"

class SphereAreaLight : public Light<SphereAreaLightBuffer>
{
private:
public:
	SphereAreaLight();
	SphereAreaLight(std::shared_ptr<VulkanEngine> engine);
	SphereAreaLight(SphereAreaLightBuffer light);
	~SphereAreaLight();

	void Update() override;
	void Destroy() override;
};
