#pragma once
#include <vulkan/vulkan_core.h>
#include "VulkanEngine.h"

class VulkanBuffer
{
	private:
		VkDeviceSize BufferSize = VK_NULL_HANDLE;
		VkBuffer StagingBuffer = VK_NULL_HANDLE;
		VkDeviceMemory StagingBufferMemory = VK_NULL_HANDLE;

		uint32_t GetMemoryType(VulkanEngine& engine, uint32_t typeFilter, VkMemoryPropertyFlags properties);

	protected:
		VkBuffer Buffer = VK_NULL_HANDLE;
		VkDeviceMemory BufferMemory = VK_NULL_HANDLE;

	public:

		VulkanBuffer();
		VulkanBuffer(VulkanEngine& engine);
		~VulkanBuffer();
		VkResult CreateStagingBuffer(VulkanEngine& engine, VkDeviceSize BufferSize);
		VkResult CreateBuffer(VulkanEngine& engine, VkDeviceSize BufferSize, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
		void CopyBuffer(VulkanEngine& engine, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		void CopyToBufferMemory(VulkanEngine& engine, void* DataToCopy, VkDeviceSize BufferSize);
		void CopyToStagingBufferMemory(VulkanEngine& engine, void* DataToCopy, VkDeviceSize BufferSize);
		void DestoryStagingBuffer(VulkanEngine& engine);
		void DestoryBuffer(VulkanEngine& engine);

		VkBuffer GetBuffer() { return Buffer; }
		VkDeviceMemory GetBufferMemory() { return BufferMemory; }
		VkDeviceSize GetBufferSize() { return BufferSize; }
};

