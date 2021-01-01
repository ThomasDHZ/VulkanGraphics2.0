#pragma once
#include <vulkan/vulkan_core.h>
#include "VulkanEngine.h"

class VulkanBuffer
{
	private:
		
		uint32_t GetMemoryType(VulkanEngine& engine, uint32_t typeFilter, VkMemoryPropertyFlags properties);

	protected:
		VkBuffer StagingBuffer = VK_NULL_HANDLE;
		VkDeviceMemory StagingBufferMemory = VK_NULL_HANDLE;
		VkBuffer Buffer = VK_NULL_HANDLE;
		VkDeviceMemory BufferMemory = VK_NULL_HANDLE;

	public:
		VulkanBuffer();
		VulkanBuffer(VulkanEngine& engine);
		VkCommandBuffer BeginSingleTimeCommand(VulkanEngine& engine);
		void EndSingleTimeCommand(VulkanEngine& engine, VkCommandBuffer commandBuffer);
		VkResult CreateStagingBuffer(VulkanEngine& engine, VkDeviceSize BufferSize);
		VkResult CreateBuffer(VulkanEngine& engine, VkDeviceSize BufferSize, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
		void CopyBuffer(VulkanEngine& renderer, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		void MapMemory(VulkanEngine& engine, void* DataToCopy, VkDeviceSize BufferSize);
		void DestoryStagingBuffer(VulkanEngine& engine);
		void DestoryBuffer(VulkanEngine& engine);
};

