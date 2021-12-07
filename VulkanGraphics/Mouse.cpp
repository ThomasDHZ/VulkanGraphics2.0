#include "Mouse.h"
#include "ObjectViewCamera.h"

void Mouse::Update(std::shared_ptr<VulkanWindow> window, std::shared_ptr<Camera> ActiveCamera)
{

	if (auto PCamera = dynamic_cast<PerspectiveCamera*>(ActiveCamera.get()))
	{
		if (glfwGetMouseButton(window->GetWindowPtr(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		{
			glfwGetCursorPos(window->GetWindowPtr(), &MouseXPos, &MouseYPos);
			if (firstMouse)
			{
				lastX = MouseXPos;
				lastY = MouseYPos;
				firstMouse = false;
			}

			float xoffset = MouseXPos - lastX;
			float yoffset = lastY - MouseYPos;

			lastX = MouseXPos;
			lastY = MouseYPos;

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
			glfwGetCursorPos(window->GetWindowPtr(), &MouseXPos, &MouseYPos);
			if (firstMouse)
			{
				lastX = MouseXPos;
				lastY = MouseYPos;
				firstMouse = false;
			}

			float xoffset = MouseXPos - lastX;
			float yoffset = lastY - MouseYPos;

			lastX = MouseXPos;
			lastY = MouseYPos;

			ObjCamera->ProcessMouseMovement(xoffset, yoffset);
		}
		else
		{
			firstMouse = true;
		}
	}
}