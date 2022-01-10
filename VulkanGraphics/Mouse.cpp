#include "Mouse.h"
#include "ObjectViewCamera.h"

Mouse Mouse::MouseListener = Mouse();

Mouse Mouse::mouse()
{
	return MouseListener;
}

void Mouse::MousePosCallback(GLFWwindow* window, double xpos, double ypos)
{
	MouseListener.LastX = MouseListener.XPos;
	MouseListener.LastY = MouseListener.YPos;
	MouseListener.XPos = xpos;
	MouseListener.YPos = ypos;
	MouseListener.IsDragging = MouseListener.MouseButtonPressed[0] || MouseListener.MouseButtonPressed[1] || MouseListener.MouseButtonPressed[2];
}

void Mouse::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (button < 3)
		{
			MouseListener.MouseButtonPressed[button] = true;
		}
	}
	else if (action == GLFW_RELEASE)
	{
		if (button < 3)
		{
			MouseListener.MouseButtonPressed[button] = false;
			MouseListener.IsDragging = false;
		}
	}
}

void Mouse::MouseScrollCallback(GLFWwindow* window, double Xoffset, double Yoffset)
{
	MouseListener.ScrollX = Xoffset;
	MouseListener.ScrollY = Yoffset;
}

Mouse::Mouse()
{
}

void Mouse::Update(std::shared_ptr<VulkanWindow> window, std::shared_ptr<Camera> ActiveCamera)
{

	if (auto PCamera = dynamic_cast<PerspectiveCamera*>(ActiveCamera.get()))
	{
		if (glfwGetMouseButton(window->GetWindowPtr(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		{
			glfwGetCursorPos(window->GetWindowPtr(), &XPos, &YPos);
			if (firstMouse)
			{
				LastX = XPos;
				LastY = YPos;
				firstMouse = false;
			}

			float xoffset = XPos - LastX;
			float yoffset = LastY - YPos;

			LastX = XPos;
			LastY = YPos;

			PCamera->ProcessMouseMovement(xoffset, yoffset);
		}
		else
		{
			firstMouse = true;
		}
	}
	else if (auto ObjCamera = dynamic_cast<ObjectViewCamera*>(ActiveCamera.get()))
	{
		if (glfwGetMouseButton(window->GetWindowPtr(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		{
			glfwGetCursorPos(window->GetWindowPtr(), &XPos, &YPos);
			if (firstMouse)
			{
				LastX = XPos;
				LastY = YPos;
				firstMouse = false;
			}

			float xoffset = XPos - LastX;
			float yoffset = LastY - YPos;

			LastX = XPos;
			LastY = YPos;

			ObjCamera->ProcessMouseMovement(xoffset, yoffset);
		}
		else
		{
			firstMouse = true;
		}
	}
}