#include "Keyboard.h"
#include "OrthographicCamera.h"

void Keyboard::Update(GLFWwindow* window, std::shared_ptr<Camera> camera)
{
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	if (auto PCamera = dynamic_cast<PerspectiveCamera*>(camera.get()))
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			PCamera->ProcessKeyboard(FORWARD, deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			PCamera->ProcessKeyboard(BACKWARD, deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			PCamera->ProcessKeyboard(LEFT, deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			PCamera->ProcessKeyboard(RIGHT, deltaTime);
		}
	}
	else if(auto OCamera = dynamic_cast<OrthographicCamera*>(camera.get()))
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			OCamera->ProcessKeyboard(UP, deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			OCamera->ProcessKeyboard(DOWN, deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			OCamera->ProcessKeyboard(LEFT, deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			OCamera->ProcessKeyboard(RIGHT, deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		{
			OCamera->ProcessKeyboard(FORWARD, deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		{
			OCamera->ProcessKeyboard(BACKWARD, deltaTime);
		}
	}
}
