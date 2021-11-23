#include "OrthographicLightViewCamera.h"

OrthographicLightViewCamera::OrthographicLightViewCamera() : OrthographicCamera()
{
}

OrthographicLightViewCamera::OrthographicLightViewCamera(std::string Name, glm::vec3 Position, glm::vec3 LightPos) : OrthographicCamera()
{
	cameraType = CameraType::Orthographic_Camera;

	Position = glm::vec3(0.0f);
	ProjectionMatrix = glm::mat4(1.0f);
	ViewMatrix = glm::mat4(1.0f);

	Update(Position, LightPos);
}

void OrthographicLightViewCamera::Update(glm::vec3 Position, glm::vec3 LightPos)
{
    float near_plane = -10000.0f, far_plane = 10000.0f;
    ProjectionMatrix = glm::ortho(-500.0f, 500.0f, -500.0f, 500.0f, near_plane, far_plane);
	ViewMatrix = glm::lookAt(LightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
}

void OrthographicLightViewCamera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
}
