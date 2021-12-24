#pragma once
#include "Light.h"
#include "CameraManager.h"
#include "OrthographicLightViewCamera.h"
#include "DirectionalShadowCamera.h"


class DirectionalLight : public Light<DirectionalLightBuffer>
{
private:

public:
	//float farPlane = 3000.0f;
	//ProjectionMatrix = glm::ortho(-3000.0f, 3000.0f, -3000.0f, 3000.0f, 0.1f, farPlane);
	glm::vec2 LeftRight = glm::vec2(-3000.0f, 3000.0f);
	glm::vec2 TopBottom = glm::vec2(-3000.0f, 3000.0f);
	glm::vec2 NearFar = glm::vec2(0.1f, 3000.0f);

	DirectionalLight();
	DirectionalLight(DirectionalLightBuffer light);
	~DirectionalLight();

	std::shared_ptr<OrthographicLightViewCamera> lightViewCamera;
	void Update() override;
	void Destroy() override;
};

