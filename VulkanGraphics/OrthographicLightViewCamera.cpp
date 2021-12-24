#include "OrthographicLightViewCamera.h"

OrthographicLightViewCamera::OrthographicLightViewCamera() : OrthographicCamera()
{
}

OrthographicLightViewCamera::OrthographicLightViewCamera(std::string Name, glm::vec3 Position, glm::vec3 LightPos) : OrthographicCamera(Name)
{
	cameraType = CameraType::Orthographic_Camera;

	Position = glm::vec3(0.0f);
	ProjectionMatrix = glm::mat4(1.0f);
	ViewMatrix = glm::mat4(1.0f);

	Update(Position, LightPos);
}

void OrthographicLightViewCamera::Update(glm::vec3 Position, glm::vec3 LightPos)
{
	
	float farPlane = 3000.0f;
	ProjectionMatrix = glm::ortho(-3000.0f, 3000.0f, -3000.0f, 3000.0f, 0.1f, farPlane);
	ViewMatrix = glm::lookAt(Position * LightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
}

void OrthographicLightViewCamera::Update(glm::vec3 LightPos, float left, float right, float bottom, float top, float zNear, float zFar)
{
	ProjectionMatrix = glm::ortho(left, right, bottom, top, zNear, zFar);
	ViewMatrix = glm::lookAt(LightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
}

void OrthographicLightViewCamera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
}
