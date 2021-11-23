#pragma once
#include "PerspectiveCamera.h"
class DirectionalShadowCamera : public PerspectiveCamera
{
public:
    DirectionalShadowCamera(std::string Name, glm::vec2 ScreenSize, glm::vec3 position);
    ~DirectionalShadowCamera();

    virtual void ProcessKeyboard(Camera_Movement direction, float deltaTime) override;
    virtual void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true) override;
    virtual void MouseScroll(float yoffset) override;
    virtual void Update(glm::vec3 position, glm::vec3 direction);
};

