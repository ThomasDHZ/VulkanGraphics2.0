#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <array>
#include "VulkanWindow.h"

class VulkanEngine
{
private:
public:
	VkInstance Instance = VK_NULL_HANDLE;
	VkDevice Device = VK_NULL_HANDLE;
	VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;
	VkSurfaceKHR Surface = VK_NULL_HANDLE;
	VkQueue GraphicsQueue = VK_NULL_HANDLE;
	VkQueue PresentQueue = VK_NULL_HANDLE;
	VkCommandPool RenderCommandPool = VK_NULL_HANDLE;

	VulkanEngine();
	VulkanEngine(VulkanWindow window);
	~VulkanEngine();
};

