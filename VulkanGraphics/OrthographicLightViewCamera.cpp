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
	ViewMatrix = glm::lookAt(LightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
	ProjectionMatrix = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f);
	ProjectionMatrix[1][1] *= -1;

	LightSpaceMatrix = ProjectionMatrix * ViewMatrix;
	EnginePtr::GetEnginePtr()->Mat4Logger("LightView", ViewMatrix);
}

void OrthographicLightViewCamera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
}
