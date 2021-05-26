#include "LightViewCamera.h"

LightViewCamera::LightViewCamera()
{

}

LightViewCamera::LightViewCamera(VulkanEngine& engine, glm::vec3 lightPos, glm::vec3 lightview)
{
	Up = glm::vec3(0.0f, 1.0f, 0.0f);
	Front = glm::vec3(0.0f, 0.0f, -1.0f);
	cameraType = CameraType::Perspective_Camera;

	Position = lightPos;
	WorldUp = Up;

	Zoom = 45.0f;
	Yaw = -90.0f;
	Pitch = 0.0f;
}

LightViewCamera::~LightViewCamera()
{
}

void LightViewCamera::Update(VulkanEngine& engine)
{
	std::cout << "Use other LightViewCamera.Update" << std::endl;
}

void LightViewCamera::Update(VulkanEngine& engine, glm::vec3 lightPos, glm::vec3 lightview)
{
	glm::vec3 front = lightview * -360.0f;
	Front = glm::normalize(front);

	Right = glm::normalize(glm::cross(Front, WorldUp));
	Up = glm::normalize(glm::cross(Right, Front));

	ViewMatrix = glm::lookAt(Position, Position + Front, Up);

	const auto Aspect = engine.SwapChain.GetSwapChainResolution().width / (float)engine.SwapChain.GetSwapChainResolution().height;
	ProjectionMatrix = glm::perspective(glm::radians(Zoom), Aspect, 0.1f, 10000.0f);
}

void LightViewCamera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
}
