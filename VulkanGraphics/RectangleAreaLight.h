#pragma once
#include "Light.h"
#include "LightViewCamera.h"
#include "CameraManager.h"

class RectangleAreaLight : public Light<RectangleAreaLightBuffer>
{
private:
public:
	RectangleAreaLight();
	RectangleAreaLight(std::shared_ptr<VulkanEngine> engine);
	RectangleAreaLight(RectangleAreaLightBuffer light);
	~RectangleAreaLight();

	void Update() override;
	void Destroy() override;
};

