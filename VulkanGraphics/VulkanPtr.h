#pragma once
#include <memory>
#include <iostream>
#include <vulkan/vulkan.h>
#include <stdexcept>
#include "VulkanWindow.h"
#include <GLFW/glfw3.h>

class VulkanPtr
{
private:
	static VkInstance Instance;
	static VkDevice Device;
	static VkPhysicalDevice PhysicalDevice;
	static VkSurfaceKHR Surface;
	static VkQueue GraphicsQueue;
	static VkQueue PresentQueue;
	static VkCommandPool CommandPool;
public:
	static void SetVulkanInstance(VkInstance instance)
	{
		if (Instance == VK_NULL_HANDLE)
		{
			Instance = instance;
		}
		else
		{
			std::cout << "Instance already Set" << std::endl;
		}
	}

	static void SetPhysicalDevice(VkPhysicalDevice physicalDevice)
	{
		if (PhysicalDevice == VK_NULL_HANDLE)
		{
			PhysicalDevice = physicalDevice;
		}
		else
		{
			std::cout << "Present Queue already Set" << std::endl;
		}
	}

	static void SetDeviceInstance(VkDevice device)
	{
		if (Device == VK_NULL_HANDLE)
		{
			Device = device;
		}
		else
		{
			std::cout << "Device already Set" << std::endl;
		}
	}

	static void SetWindowSurface(VkSurfaceKHR surface)
	{
		if (Surface == VK_NULL_HANDLE)
		{
			Surface = surface;
		}
		else
		{
			std::cout << "Window Surface already Set" << std::endl;
		}
	}

	static void SetGraphicsQueue(VkQueue graphicsQueue)
	{
		if (GraphicsQueue == VK_NULL_HANDLE)
		{
			GraphicsQueue = graphicsQueue;
		}
		else
		{
			std::cout << "Present Queue already Set" << std::endl;
		}
	}

	static void SetPresentQueue(VkQueue presentQueue)
	{
		if (PresentQueue == VK_NULL_HANDLE)
		{
			PresentQueue = presentQueue;
		}
		else
		{
			std::cout << "Present Queue already Set" << std::endl;
		}
	}

	static void SetCommandPool(VkCommandPool commandPool)
	{
		if (CommandPool == VK_NULL_HANDLE)
		{
			CommandPool = commandPool;
		}
		else
		{
			std::cout << "Command Pool already Set" << std::endl;
		}
	}

	static VkInstance GetVulkanInstance() { return Instance; };
	static VkDevice GetDevice() { return Device; };
	static VkPhysicalDevice GetPhysicalDevice() { return PhysicalDevice; };
	static VkSurfaceKHR GetSurface() { return Surface; };
	static VkQueue GetGraphicsQueue() { return GraphicsQueue; };
	static VkQueue GetPresentQueue() { return PresentQueue; };
	static VkCommandPool GetCommandPool() { return CommandPool; };
};
