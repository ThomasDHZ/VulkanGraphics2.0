#pragma once
#include "Light.h"
#include "LightViewCamera.h"
#include "CameraManager.h"
#include "OrthographicLightViewCamera.h"


class DirectionalLight : public Light<DirectionalLightBuffer>
{
private:

public:
	DirectionalLight();
	DirectionalLight(DirectionalLightBuffer light);
	~DirectionalLight();

	std::shared_ptr<OrthographicLightViewCamera> lightViewCamera;
	void Update() override;
	void Destroy() override;
};
