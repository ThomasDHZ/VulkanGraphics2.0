#ifndef VulkanEngine_H
#define VulkanEngine_H
#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <array>
#include "VulkanWindow.h"
#include "VulkanSwapChain.h"
#include "VulkanDebugger.h"
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "VulkanBuffer.h"
#include <chrono>

#define VKB_VALIDATION_LAYERS

const int MAX_FRAMES_IN_FLIGHT = 2;

struct DescriptorSetLayoutBindingInfo
{
	uint32_t Binding;
	VkDescriptorType DescriptorType;
	VkShaderStageFlags StageFlags;
	uint32_t Count;
};

struct StorageImage {
	VkDeviceMemory memory = VK_NULL_HANDLE;
	VkImage image = VK_NULL_HANDLE;
	VkImageView view = VK_NULL_HANDLE;
	VkFormat format;
};

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

	uint32_t NextID = 0;

	void SetUpDeviceFeatures(GLFWwindow* window);

	void FindQueueFamilies(VkPhysicalDevice PhysicalDevice, VkSurfaceKHR Surface);
	bool checkDeviceExtensionSupport(VkPhysicalDevice GPUDevice);

	void GetInstanceLayerProperties();
	void GetPhysicalDevice();
	bool isDeviceSuitable(VkPhysicalDevice GPUDevice);
	std::vector<const char*> getRequiredExtensions();

	void InitializeCommandPool();
	void InitializeSyncObjects();

	VkShaderModule ReadShaderFile(const std::string& filename);

public:
	VkInstance Instance = VK_NULL_HANDLE;
	VkDevice Device = VK_NULL_HANDLE;
	VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;
	VkSurfaceKHR Surface = VK_NULL_HANDLE;
	VkQueue GraphicsQueue = VK_NULL_HANDLE;
	VkQueue PresentQueue = VK_NULL_HANDLE;
	VkDescriptorPool      DescriptorPool = VK_NULL_HANDLE;
	VkCommandPool CommandPool = VK_NULL_HANDLE;
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

	uint32_t GenerateID();
	float    VulkanTimer();

	VkPhysicalDeviceFeatures GetPhysicalDeviceFeatures(VkPhysicalDevice GPUDevice);
	std::vector<VkSurfaceFormatKHR> GetSurfaceFormatList(VkPhysicalDevice GPUDevice);
	std::vector<VkPresentModeKHR> GetPresentModeList(VkPhysicalDevice GPUDevice, VkSurfaceKHR Surface);


	void Destroy();

	VkCommandBuffer beginSingleTimeCommands();
	VkCommandBuffer beginSingleTimeCommands(VkCommandPool& commandPool);
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);
	void endSingleTimeCommands(VkCommandBuffer commandBuffer, VkCommandPool& commandPool);

	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	uint64_t GetBufferDeviceAddress(VkBuffer buffer);
	VkPipelineShaderStageCreateInfo CreateShader(const std::string& filename, VkShaderStageFlagBits shaderStages);

	VkDescriptorPoolSize AddDsecriptorPoolBinding(VkDescriptorType descriptorType);
	VkDescriptorPool CreateDescriptorPool(std::vector<VkDescriptorPoolSize> DescriptorPoolInfo);
	VkDescriptorSetLayout CreateDescriptorSetLayout(std::vector<DescriptorSetLayoutBindingInfo> LayoutBindingInfo);
	VkDescriptorSet CreateDescriptorSets(VkDescriptorPool descriptorPool, VkDescriptorSetLayout layout);
	VkDescriptorBufferInfo AddBufferDescriptor(VulkanBuffer& buffer);

	VkWriteDescriptorSet AddAccelerationBuffer(unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, VkWriteDescriptorSetAccelerationStructureKHR& accelerationStructure);
	VkWriteDescriptorSet AddStorageImageBuffer(unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, VkDescriptorImageInfo& TextureImageInfo);
	VkWriteDescriptorSet AddBufferDescriptorSet(unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, VkDescriptorBufferInfo& BufferInfo, VkDescriptorType descriptorType);
	VkWriteDescriptorSet AddBufferDescriptorSet(unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, std::vector<VkDescriptorBufferInfo>& BufferInfoList, VkDescriptorType descriptorType);
	VkWriteDescriptorSet AddTextureDescriptorSet(unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, VkDescriptorImageInfo& TextureImageInfo);
	VkWriteDescriptorSet AddTextureDescriptorSet(unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, std::vector<VkDescriptorImageInfo>& TextureImageInfo);
	VkWriteDescriptorSetAccelerationStructureKHR AddAcclerationStructureBinding(VkAccelerationStructureKHR& handle);
	VkDescriptorImageInfo AddRayTraceReturnImageDescriptor(VkImageLayout ImageLayout, StorageImage& texture);
	VkDescriptorImageInfo AddTextureDescriptor(VkImageView view, VkSampler sampler);

	uint32_t GetAlignedSize(uint32_t value, uint32_t alignment);
	void Mat4Logger(std::string name, glm::mat4 matrix);

	PFN_vkGetBufferDeviceAddressKHR vkGetBufferDeviceAddressKHR;
	PFN_vkCreateAccelerationStructureKHR vkCreateAccelerationStructureKHR;
	PFN_vkDestroyAccelerationStructureKHR vkDestroyAccelerationStructureKHR;
	PFN_vkGetAccelerationStructureBuildSizesKHR vkGetAccelerationStructureBuildSizesKHR;
	PFN_vkGetAccelerationStructureDeviceAddressKHR vkGetAccelerationStructureDeviceAddressKHR;
	PFN_vkCmdBuildAccelerationStructuresKHR vkCmdBuildAccelerationStructuresKHR;
	PFN_vkBuildAccelerationStructuresKHR vkBuildAccelerationStructuresKHR;
	PFN_vkCmdTraceRaysKHR vkCmdTraceRaysKHR;
	PFN_vkGetRayTracingShaderGroupHandlesKHR vkGetRayTracingShaderGroupHandlesKHR;
	PFN_vkCreateRayTracingPipelinesKHR vkCreateRayTracingPipelinesKHR;

	VkInstance GetVulkanInstance() { return Instance; }
	VkDevice GetVulkanDevice() { return Device; }
	VkPhysicalDevice GetVulkanPhysicalDevice() { return PhysicalDevice; }
	VkSurfaceKHR GetVulkanSurface() { return Surface; }
	VkQueue GetVulkanGraphicsQueue() { return GraphicsQueue; }
	VkQueue GetVulkanPresentQueue() { return PresentQueue; }
	VkCommandPool GetRenderCommandPool() { return CommandPool; }
	VkSwapchainKHR GetSwapChain() { return SwapChain.GetSwapChain(); }
	std::vector<VkImage> GetSwapChainImages() { return SwapChain.GetSwapChainImages(); }
	std::vector<VkImageView> GetSwapChainImageViews() { return SwapChain.GetSwapChainImageViews(); }
	VkExtent2D GetSwapChainResolution() { return SwapChain.GetSwapChainResolution(); }
	VkSurfaceFormatKHR GetSwapChainImageFormat() { return SwapChain.GetSwapChainImageFormat(); }
	uint32_t GetSwapChainMinImageCount() { return SwapChain.GetSwapChainMinImageCount(); }
	uint32_t GetSwapChainImageCount() { return SwapChain.GetSwapChainImageCount(); }
};
#endif