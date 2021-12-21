#pragma once
#include "Light.h"
#include "OrthographicLightViewCamera.h"

class PointLight : public Light<PointLightBuffer>
{
private:

public:

	glm::vec3 cameraDirection = glm::vec3(0.0f);

	PointLight();
	PointLight(PointLightBuffer light);
	~PointLight();

	void Update() override;
	void Destroy() override;

	std::shared_ptr<OrthographicLightViewCamera> lightViewCamera;
	glm::mat3 GetLightSpaceMatrix() { return lightViewCamera->GetLightSpaceMatrix(); }
};

