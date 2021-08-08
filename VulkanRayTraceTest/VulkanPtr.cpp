#include "VulkanPtr.h"

 VkInstance VulkanPtr::Instance = VK_NULL_HANDLE;
 VkDevice VulkanPtr::Device = VK_NULL_HANDLE;
 VkPhysicalDevice VulkanPtr::PhysicalDevice = VK_NULL_HANDLE;
 VkSurfaceKHR VulkanPtr::Surface = VK_NULL_HANDLE;
 VkQueue VulkanPtr::GraphicsQueue = VK_NULL_HANDLE;
 VkQueue VulkanPtr::PresentQueue = VK_NULL_HANDLE;
 VkCommandPool VulkanPtr::CommandPool = VK_NULL_HANDLE;