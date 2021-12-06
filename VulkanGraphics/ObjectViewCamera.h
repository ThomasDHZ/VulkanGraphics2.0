#pragma once
#include "Camera.h"

class ObjectViewCamera : public Camera
{
private:
protected:
    float YAW = -90.0f;
    float PITCH = 0.0f;

    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float MovementSpeed;
    float MouseSensitivity;

public:

    float Yaw;
    float Pitch;
    glm::mat3 LightSpaceMatrix = glm::mat3(1.0f);

	ObjectViewCamera();
    ObjectViewCamera(std::string Name, glm::vec2 ScreenSize, glm::vec3 position);
    ObjectViewCamera(std::string Name, glm::vec2 ScreenSize, glm::vec3 position, float pitch, float yaw);
	~ObjectViewCamera();

    virtual void Update(std::shared_ptr<VulkanEngine> engine);
    void Update(glm::vec3 position);
    void Update(glm::vec3 position, glm::vec3 direction);

    glm::mat3 GetLightSpaceMatrix() { return LightSpaceMatrix; };
};

