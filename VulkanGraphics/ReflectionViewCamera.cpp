#include "ReflectionViewCamera.h"

ReflectionViewCamera::ReflectionViewCamera(std::string Name, glm::vec2 ScreenSize, glm::vec3 position) : PerspectiveCamera(Name, ScreenSize, position)
{
}

ReflectionViewCamera::~ReflectionViewCamera()
{
}

void ReflectionViewCamera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
}

void ReflectionViewCamera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
	PerspectiveCamera::ProcessMouseMovement(xoffset, yoffset, constrainPitch);
}

void ReflectionViewCamera::MouseScroll(float yoffset)
{
}

void ReflectionViewCamera::Update(std::shared_ptr<VulkanEngine> engine)
{
	PerspectiveCamera::Update(engine);
}
