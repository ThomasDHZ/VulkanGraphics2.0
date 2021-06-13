#pragma once
#include "Camera.h"
#include <GLFW\glfw3.h>
#include "PerspectiveCamera.h"
#include <memory>

class Mouse
{
private:
	float lastX;
	float lastY;
	bool firstMouse;
	double MouseXPos;
	double MouseYPos;

public:
	void Update(std::shared_ptr<VulkanWindow> window, std::shared_ptr<Camera> ActiveCamera);
	glm::vec2 GetMousePos() { return glm::vec2(MouseXPos, MouseYPos); }
};