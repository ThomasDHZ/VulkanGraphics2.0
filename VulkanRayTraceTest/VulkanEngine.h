#ifndef VulkanEngine_H
#define VulkanEngine_H
#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <array>
#include "VulkanWindow.h"
#include "VulkanDebugger.h"
#include "VulkanSwapChain.h"

const int MAX_FRAMES_IN_FLIGHT = 2;

class VulkanEngine
{
private:

	struct VulkanSemaphores
	{
		VkSemaphore ImageAcquiredSemaphore;
		VkSemaphore RenderCompleteSemaphore;

		void Destory(VkDevice device)
		{
			vkDestroySemaphore(device, RenderCompleteSemaphore, nullptr);
			vkDestroySemaphore(device, ImageAcquiredSemaphore, nullptr);

			RenderCompleteSemaphore = VK_NULL_HANDLE;
			ImageAcquiredSemaphore = VK_NULL_HANDLE;
		}
	};

	std::vector<const char*> ValidationLayers;
	std::vector<const char*> DeviceExtensions;



	VulkanDebugger VulkanDebug;

	void SetUPDeviceFeatures(GLFWwindow* window);

	void FindQueueFamilies(VkPhysicalDevice PhysicalDevice, VkSurfaceKHR Surface);
	bool checkDeviceExtensionSupport(VkPhysicalDevice GPUDevice);

	void GetInstanceLayerProperties();
	void GetPhysicalDevice();
	bool isDeviceSuitable(VkPhysicalDevice GPUDevice);
	std::vector<const char*> getRequiredExtensions();

	void InitializeCommandPool();
	void InitializeSyncObjects();

public:
	VulkanWindow vulkanWindow;
	VkInstance Instance = VK_NULL_HANDLE;
	VkDevice Device = VK_NULL_HANDLE;
	VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;
	VkSurfaceKHR Surface = VK_NULL_HANDLE;
	VkQueue GraphicsQueue = VK_NULL_HANDLE;
	VkQueue PresentQueue = VK_NULL_HANDLE;
	VkDescriptorPool      DescriptorPool = VK_NULL_HANDLE;
	VkCommandPool RenderCommandPool = VK_NULL_HANDLE;
	VulkanSwapChain SwapChain;

	std::vector<VulkanSemaphores> vulkanSemaphores;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;

	int GraphicsFamily = -1;
	int PresentFamily = -1;
	uint32_t DrawFrame = 0;

	std::vector<VkLayerProperties> VulkanLayers;

	VulkanEngine();
	VulkanEngine(GLFWwindow* window);
	~VulkanEngine();

	VkPhysicalDeviceFeatures GetPhysicalDeviceFeatures(VkPhysicalDevice GPUDevice);
	std::vector<VkSurfaceFormatKHR> GetSurfaceFormatList(VkPhysicalDevice GPUDevice);
	std::vector<VkPresentModeKHR> GetPresentModeList(VkPhysicalDevice GPUDevice, VkSurfaceKHR Surface);

	void Destory();

	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);
	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	VkInstance GetVulkanInstance() { return Instance; }
	VkDevice GetVulkanDevice() { return Device; }
	VkPhysicalDevice GetVulkanPhysicalDevice() { return PhysicalDevice; }
	VkSurfaceKHR GetVulkanSurface() { return Surface; }
	VkQueue GetVulkanGraphicsQueue() { return GraphicsQueue; }
	VkQueue GetVulkanPresentQueue() { return PresentQueue; }
	VkCommandPool GetRenderCommandPool() { return RenderCommandPool; }
	VkSwapchainKHR GetSwapChain() { return SwapChain.GetSwapChain(); }
	std::vector<VkImage> GetSwapChainImages() { return SwapChain.GetSwapChainImages(); }
	std::vector<VkImageView> GetSwapChainImageViews() { return SwapChain.GetSwapChainImageViews(); }
	VkExtent2D GetSwapChainResolution() { return SwapChain.GetSwapChainResolution(); }
	VkSurfaceFormatKHR GetSwapChainImageFormat() { return SwapChain.GetSwapChainImageFormat(); }
	uint32_t GetSwapChainMinImageCount() { return SwapChain.GetSwapChainMinImageCount(); }
	uint32_t GetSwapChainImageCount() { return SwapChain.GetSwapChainImageCount(); }
};
#endif