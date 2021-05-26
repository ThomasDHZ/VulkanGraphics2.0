#pragma once
#include "Camera.h"
#include "LightViewCamera.h"

class LightViewCamera : public Camera
{
private:
    float YAW = -90.0f;
    float PITCH = 0.0f;

    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

public:

    float Yaw;
    float Pitch;

	LightViewCamera();
	LightViewCamera(VulkanEngine& engine, glm::vec3 lightPos, glm::vec3 lightview);
	~LightViewCamera();

	void Update(VulkanEngine& engine) override;
	void Update(VulkanEngine& engine, glm::vec3 lightPos, glm::vec3 lightview);
	void ProcessKeyboard(Camera_Movement direction, float deltaTime) override;
};