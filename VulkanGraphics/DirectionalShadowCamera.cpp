#include "DirectionalShadowCamera.h"

DirectionalShadowCamera::DirectionalShadowCamera(std::string Name, glm::vec2 ScreenSize, glm::vec3 position) : PerspectiveCamera(Name, ScreenSize, position)
{
}

DirectionalShadowCamera::~DirectionalShadowCamera()
{
}

void DirectionalShadowCamera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
}

void DirectionalShadowCamera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{

}

void DirectionalShadowCamera::MouseScroll(float yoffset)
{
}

void DirectionalShadowCamera::Update(glm::vec3 position, glm::vec3 direction)
{
    glm::vec3 front = glm::vec3(0.0f);
    front.x = cos(glm::radians(direction.z)) * cos(glm::radians(direction.y));
    front.y = sin(glm::radians(direction.y));
    front.z = sin(glm::radians(direction.z)) * cos(glm::radians(direction.y));
    Front = glm::normalize(front);

    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));

    ViewMatrix = glm::lookAt(position, position + Front, Up);

    const auto Aspect = EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainResolution().width / (float)EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainResolution().height;
    ProjectionMatrix = glm::perspective(glm::radians(Zoom), Aspect, 0.1f, 10000.0f);
    ProjectionMatrix[1][1] *= -1;
}
