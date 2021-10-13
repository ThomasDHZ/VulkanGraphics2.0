#pragma once
#include "Light.h"
#include "OrthographicLightViewCamera.h"
class PointLight : public Light<PointLightBuffer>
{
private:
	std::shared_ptr<OrthographicLightViewCamera> lightViewCamera;

public:
	PointLight();
	PointLight(PointLightBuffer light);
	~PointLight();

	void Update() override;
	void Destroy() override;

	glm::mat3 GetLightSpaceMatrix() { return lightViewCamera->GetLightSpaceMatrix(); }
};

