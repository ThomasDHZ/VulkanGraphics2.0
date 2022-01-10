#pragma once
#include "Camera.h"
#include <GLFW\glfw3.h>
#include "PerspectiveCamera.h"
#include <memory>

class Mouse
{
private:
	bool firstMouse;
	double ScrollX;
	double ScrollY;
	double XPos;
	double YPos;
	double LastX;
	double LastY;
	bool MouseButtonPressed[3];
	bool IsDragging;

	static Mouse MouseListener;
	const int MOUSEBUTTONS = 3;
public:
	static Mouse mouse();
	static void MousePosCallback(GLFWwindow* window, double xpos, double ypos);
	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void MouseScrollCallback(GLFWwindow* window, double Xoffset, double Yoffset);

	Mouse();
	void Update(std::shared_ptr<VulkanWindow> window, std::shared_ptr<Camera> ActiveCamera);
	glm::vec2 GetMousePos() { return glm::vec2(XPos, YPos); }
};