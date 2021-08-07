#pragma once
#include <GLFW/glfw3.h>
#include <memory>

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
