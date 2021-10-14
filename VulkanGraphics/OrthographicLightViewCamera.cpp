#include "OrthographicLightViewCamera.h"

OrthographicLightViewCamera::OrthographicLightViewCamera() : OrthographicCamera()
{
}

OrthographicLightViewCamera::OrthographicLightViewCamera(glm::vec3 LightPos) : OrthographicCamera()
{
	cameraType = CameraType::Orthographic_Camera;

	Position = glm::vec3(0.0f);
	ProjectionMatrix = glm::mat4(1.0f);
	ViewMatrix = glm::mat4(1.0f);

	Update(LightPos);
}

void OrthographicLightViewCamera::Update(glm::vec3 LightPos)
{
    float near_plane = -10.0f, far_plane = 10.0f;

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 4.0f, -1.0f));
    transform = glm::rotate(transform, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    ViewMatrix = glm::inverse(transform);
    ProjectionMatrix = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
}

void OrthographicLightViewCamera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
}
