#include "OrthographicCamera.h"
#include <glm/gtc/matrix_transform.hpp>

OrthographicCamera::OrthographicCamera() : Camera()
{
	Position = glm::vec3(0.0f);
	ProjectionMatrix = glm::mat4(1.0f);
	ViewMatrix = glm::mat4(1.0f);
}

OrthographicCamera::OrthographicCamera(std::string Name) : Camera(Name)
{
	Position = glm::vec3(0.0f);
	ProjectionMatrix = glm::mat4(1.0f);
	ViewMatrix = glm::mat4(1.0f);
}

OrthographicCamera::OrthographicCamera(std::string Name, float width, float height) : Camera(Name)
{
	Width = width;
	Height = height;
	Aspect = Width / Height;
	Zoom = 1.0f;
	cameraType = CameraType::Orthographic_Camera;

	Position = glm::vec3(0.0f);
	ViewScreenSize = glm::vec2(width, height);
	ProjectionMatrix = glm::ortho(-Aspect * Zoom, Aspect * Zoom, -1.0f * Zoom, 1.0f * Zoom, -1.0f, 1.0f);
	ViewMatrix = glm::mat4(1.0f);
}

OrthographicCamera::OrthographicCamera(std::string Name, glm::vec2 viewScreenSize) : Camera(Name)
{
	Width = viewScreenSize.x;
	Height = viewScreenSize.y;
	Aspect = Width / Height;
	Zoom = 1.0f;
	cameraType = CameraType::Orthographic_Camera;

	Position = glm::vec3(0.0f);
	ViewScreenSize = viewScreenSize;
	ProjectionMatrix = glm::ortho(-Aspect * Zoom, Aspect * Zoom, -1.0f * Zoom, 1.0f * Zoom, -1.0f, 1.0f);
	ViewMatrix = glm::mat4(1.0f);
}

OrthographicCamera::OrthographicCamera(std::string Name, float width, float height, float zoom) : Camera(Name)
{
	Width = width;
	Height = height;
	Aspect = Width / Height;
	Zoom = zoom;
	cameraType = CameraType::Orthographic_Camera;

	Position = glm::vec3(0.0f);
	ViewScreenSize = glm::vec2(width, height);
	ProjectionMatrix = glm::ortho(-ViewScreenSize.x * Zoom, ViewScreenSize.x * Zoom, -ViewScreenSize.y * Zoom, ViewScreenSize.y * Zoom, -1.0f, 1.0f);
	ViewMatrix = glm::mat4(1.0f);
}

OrthographicCamera::OrthographicCamera(std::string Name, glm::vec2 viewScreenSize, float zoom) : Camera(Name)
{
	Width = viewScreenSize.x;
	Height = viewScreenSize.y;
	Aspect = Width / Height;
	Zoom = zoom;
	cameraType = CameraType::Orthographic_Camera;

	Position = glm::vec3(0.0f);
	ViewScreenSize = viewScreenSize;
	ProjectionMatrix = glm::ortho(-ViewScreenSize.x * Zoom, ViewScreenSize.x * Zoom, -ViewScreenSize.y * Zoom, ViewScreenSize.y * Zoom, -1.0f, 1.0f);
	ViewMatrix = glm::mat4(1.0f);
}

OrthographicCamera::OrthographicCamera(std::string Name, glm::vec2 viewScreenSize, glm::vec2 position) : Camera(Name)
{
	Width = viewScreenSize.x;
	Height = viewScreenSize.y;
	Aspect = Width / Height;
	Zoom = 1.0f;
	Position = glm::vec3(position, 0.0f);
	cameraType = CameraType::Orthographic_Camera;

	ViewScreenSize = viewScreenSize;
	ProjectionMatrix = glm::ortho(-Aspect * Zoom, Aspect * Zoom, -1.0f * Zoom, 1.0f * Zoom, -1.0f, 1.0f);
	ViewMatrix = glm::mat4(1.0f);
}

OrthographicCamera::OrthographicCamera(std::string Name, glm::vec2 viewScreenSize, glm::vec2 position, float zoom) : Camera(Name)
{
	Width = viewScreenSize.x;
	Height = viewScreenSize.y;
	Aspect = Width / Height;
	Zoom = zoom;
	Position = glm::vec3(position, 0.0f);;
	cameraType = CameraType::Orthographic_Camera;

	ViewScreenSize = viewScreenSize;
	ProjectionMatrix = glm::ortho(-ViewScreenSize.x * Zoom, ViewScreenSize.x * Zoom, -ViewScreenSize.y * Zoom, ViewScreenSize.y * Zoom, -1.0f, 1.0f);
	ViewMatrix = glm::mat4(1.0f);
}

void OrthographicCamera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
	float velocity = MovementSpeed * deltaTime;
	if (direction == FORWARD)
		Zoom += 5.0f * velocity;
	if (direction == BACKWARD)
		Zoom -= 5.0f * velocity;
	if (direction == UP)
		Position.y += 1.0f * velocity;
	if (direction == DOWN)
		Position.y -= 1.0f * velocity;
	if (direction == LEFT)
		Position.x -= 1.0f * velocity;
	if (direction == RIGHT)
		Position.x += 1.0f * velocity;
}

void OrthographicCamera::Update(std::shared_ptr<VulkanEngine> engine)
{
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), Position) * glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0, 0, 1));
	ViewMatrix = glm::inverse(transform);

	const auto 	Aspect = engine->SwapChain.GetSwapChainResolution().width / (float)engine->SwapChain.GetSwapChainResolution().height;
	ProjectionMatrix = glm::ortho(-Aspect * Zoom, Aspect * Zoom, -1.0f * Zoom, 1.0f * Zoom, -10.0f, 10.0f);
	ProjectionMatrix[1][1] *= -1;

	ViewScreenSize = glm::vec2((Aspect * Zoom) * 2, (1.0f * Zoom) * 2);
}