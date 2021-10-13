#pragma once
#include "Camera.h"
#include "Vulkanengine.h"

class OrthographicCamera : public Camera
{
public:

	OrthographicCamera();
	OrthographicCamera(float width, float height);
	OrthographicCamera(float width, float height, float zoom);
	OrthographicCamera(glm::vec2 viewScreenSize);
	OrthographicCamera(glm::vec2 viewScreenSize, float zoom);
	OrthographicCamera(glm::vec2 viewScreenSize, glm::vec2 position);
	OrthographicCamera(glm::vec2 viewScreenSize, glm::vec2 position, float zoom);

	virtual void Update(std::shared_ptr<VulkanEngine> engine);
	virtual void ProcessKeyboard(Camera_Movement direction, float deltaTime) override;
};