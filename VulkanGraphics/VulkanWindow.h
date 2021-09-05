#pragma once
#include <GLFW/glfw3.h>
#include <memory>
#include <iostream>

class VulkanWindow
{
private:


public:
	bool FramebufferResized;
	unsigned int Width;
	unsigned int Height;
	GLFWwindow* GLFWindow;
	VulkanWindow();
	VulkanWindow(unsigned int width, unsigned int height, const char* WindowName);
	~VulkanWindow();

	void Update();
	void Destroy();
	static void frameBufferResizeCallBack(GLFWwindow* Window, int width, int height);

	void SetFrameBufferResizedFlag(bool flag) { FramebufferResized = flag; }

	GLFWwindow* GetWindowPtr() { return GLFWindow; }
	unsigned int GetWindowWidth() { return Width; }
	unsigned int GetWindowHeight() { return Height; }
	bool GetFrameBufferResizedFlag() { return FramebufferResized; }
};
class WindowPtr
{
private:
	static std::shared_ptr<VulkanWindow> windowPtr;
public:
	static void SetUpPtr(unsigned int width, unsigned int height, const char* WindowName)
	{
		if (windowPtr == nullptr)
		{
			windowPtr = std::make_shared<VulkanWindow>(VulkanWindow(width, height, WindowName));
		}
		else
		{
			std::cout << "Window has already been initialized." << std::endl;
		}
	}

	static void SetUpPtr(std::shared_ptr<VulkanWindow> window)
	{
		if (windowPtr == nullptr)
		{
			windowPtr = window;
		}
		else
		{
			std::cout << "Window has already been initialized." << std::endl;
		}
	}

	static std::shared_ptr<VulkanWindow> GetWindowPtr()
	{
		return windowPtr;
	}
};