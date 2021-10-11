#pragma once
#include "Light.h"
#include "LightViewCamera.h"
#include "CameraManager.h"


class DirectionalLight : public Light<DirectionalLightBuffer>
{
private:
	glm::mat4 LightViewMatrix;
	glm::mat4 LightProjectionMatrix;

public:
	DirectionalLight();
	DirectionalLight(std::shared_ptr<VulkanEngine> engine);
	DirectionalLight(DirectionalLightBuffer light);
	~DirectionalLight();

	void Update() override;
	void Destroy() override;

	glm::mat4 GetLightViewMatrix() { return LightViewMatrix; }
	glm::mat4 GetLightProjectionMatrix() { return LightProjectionMatrix; }
};

