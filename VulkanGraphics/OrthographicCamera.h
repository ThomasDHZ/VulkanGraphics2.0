#pragma once
#include "Camera.h"
#include "Vulkanengine.h"

class OrthographicCamera : public Camera
{
public:

	OrthographicCamera();
	OrthographicCamera(std::string Name);
	OrthographicCamera(std::string Name, float width, float height);
	OrthographicCamera(std::string Name, float width, float height, float zoom);
	OrthographicCamera(std::string Name, glm::vec2 viewScreenSize);
	OrthographicCamera(std::string Name, glm::vec2 viewScreenSize, float zoom);
	OrthographicCamera(std::string Name, glm::vec2 viewScreenSize, glm::vec2 position);
	OrthographicCamera(std::string Name, glm::vec2 viewScreenSize, glm::vec2 position, float zoom);

	virtual void Update(std::shared_ptr<VulkanEngine> engine);
	virtual void ProcessKeyboard(Camera_Movement direction, float deltaTime) override;
};