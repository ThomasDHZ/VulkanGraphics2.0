#pragma once
#include "Camera.h"
#include "Vulkanengine.h"

class PerspectiveCamera : public Camera
{
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

    PerspectiveCamera(std::string Name, glm::vec2 ScreenSize, glm::vec3 position);
    PerspectiveCamera(std::string Name, glm::vec2 ScreenSize, glm::vec3 position, float pitch, float yaw);
    ~PerspectiveCamera();

    virtual void ProcessKeyboard(Camera_Movement direction, float deltaTime) override;
    virtual void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    virtual void MouseScroll(float yoffset);
    virtual void Update(std::shared_ptr<VulkanEngine> engine);

    glm::vec3 GetFront() { return Front; }
};
