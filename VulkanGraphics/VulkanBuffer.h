#pragma once
#include "VulkanEngine.h"

class VulkanBuffer
{
private:

	VkDescriptorBufferInfo BufferDescriptor;

	uint32_t GetMemoryType(VulkanEngine& engine, uint32_t typeFilter, VkMemoryPropertyFlags properties);

protected:



public:
	VkBuffer Buffer = VK_NULL_HANDLE;
	VkDeviceMemory BufferMemory = VK_NULL_HANDLE;
	VkDeviceSize BufferSize = VK_NULL_HANDLE;

	VulkanBuffer();
	~VulkanBuffer();
	VkResult CreateBuffer(VulkanEngine& engine, VkDeviceSize BufferSize, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, void* BufferData = nullptr);
	void CopyBuffer(VulkanEngine& engine, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	void CopyBufferToMemory(VulkanEngine& engine, void* DataToCopy, VkDeviceSize BufferSize);
	void DestoryBuffer(VulkanEngine& engine);
	VkCommandBuffer BeginSingleTimeCommand(VulkanEngine& engine);
	void EndSingleTimeCommand(VulkanEngine& engine, VkCommandBuffer commandBuffer);
	
	VkBuffer GetBuffer() { return Buffer; }
	VkDeviceMemory GetBufferMemory() { return BufferMemory; }
	VkDeviceSize GetBufferSize() { return BufferSize; }
};

