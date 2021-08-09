#pragma once
#include "VulkanPtr.h"

class VulkanBuffer
{
private:

	VkDescriptorBufferInfo BufferDescriptor;

	uint32_t GetMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

protected:



public:
	VkBuffer Buffer = VK_NULL_HANDLE;
	VkDeviceMemory BufferMemory = VK_NULL_HANDLE;
	VkDeviceSize BufferSize = 0;
	uint64_t BufferDeviceAddress = 0;
	VkAccelerationStructureKHR BufferHandle = VK_NULL_HANDLE;

	VulkanBuffer();
	VulkanBuffer(VkDeviceSize BufferSize, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, void* BufferData = nullptr);
	~VulkanBuffer();

	VkResult CreateBuffer(VkDeviceSize BufferSize, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, void* BufferData = nullptr);
	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	void CopyBufferToMemory(void* DataToCopy, VkDeviceSize BufferSize);
	void DestoryBuffer();
	VkCommandBuffer BeginSingleTimeCommand();
	void EndSingleTimeCommand(VkCommandBuffer commandBuffer);

	VkBuffer GetBuffer() { return Buffer; }
	VkDeviceMemory GetBufferMemory() { return BufferMemory; }
	VkDeviceSize GetBufferSize() { return BufferSize; }
};
