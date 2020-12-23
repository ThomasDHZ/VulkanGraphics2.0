#pragma once
#include "Camera.h"
#include "PerspectiveCamera.h"
#include "OrthographicCamera.h"
#include <GLFW\glfw3.h>
#include <memory>
#include "PointLight.h"

class Test
{
private:
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

public:

	void Update(GLFWwindow* window, std::shared_ptr<Camera> camera);
};