#pragma once
#include "Light.h"
#include "LightViewCamera.h"
#include "CameraManager.h"

class TubeAreaLight : public Light<TubeAreaLightBuffer>
{
private:
public:
	TubeAreaLight();
	TubeAreaLight(std::shared_ptr<VulkanEngine> engine);
	TubeAreaLight(TubeAreaLightBuffer light);
	~TubeAreaLight();

	void Update() override;
	void Destroy() override;
};
