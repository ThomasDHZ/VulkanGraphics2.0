#pragma once
#include "PerspectiveCamera.h"

class ReflectionViewCamera : public PerspectiveCamera
{
public:
    ReflectionViewCamera(std::string Name, glm::vec2 ScreenSize, glm::vec3 position);
    ~ReflectionViewCamera();

    virtual void ProcessKeyboard(Camera_Movement direction, float deltaTime) override;
    virtual void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true) override;
    virtual void MouseScroll(float yoffset) override;
    virtual void Update(std::shared_ptr<VulkanEngine> engine) override;
};

