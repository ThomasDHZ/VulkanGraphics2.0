#pragma once
#include "OrthographicCamera.h"
#include "Vulkanengine.h"

class OrthographicLightViewCamera : public OrthographicCamera
{
private:
	glm::mat3 LightSpaceMatrix = glm::mat3(1.0f);
public:

	OrthographicLightViewCamera();
	OrthographicLightViewCamera(std::string Name, glm::vec3 Position, glm::vec3 LightPos);

	void Update(glm::vec3 Position, glm::vec3 LightPos);
	void Update(glm::vec3 LightPos, float left, float right, float bottom, float top, float zNear, float zFar);
	void ProcessKeyboard(Camera_Movement direction, float deltaTime) override;

	glm::mat3 GetLightSpaceMatrix() { return LightSpaceMatrix; };
};