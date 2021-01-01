#include "VulkanBuffer.h"

uint32_t VulkanBuffer::GetMemoryType(VulkanEngine& engine, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(engine.PhysicalDevice, &memProperties);

	for (uint32_t x = 0; x < memProperties.memoryTypeCount; x++)
	{
		if ((typeFilter & (1 << x)) && (memProperties.memoryTypes[x].propertyFlags & properties) == properties) {
			return x;
		}
	}

	throw std::runtime_error("Failed to find suitable memory type.");
}

VulkanBuffer::VulkanBuffer()
{
}

VulkanBuffer::VulkanBuffer(VulkanEngine& engine)
{
}

VkResult VulkanBuffer::CreateStagingBuffer(VulkanEngine& engine, VkDeviceSize BufferSize)
{
	VkBufferCreateInfo buffer = {};
	buffer.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer.size = BufferSize;
	buffer.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	buffer.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	if (vkCreateBuffer(engine.Device, &buffer, nullptr, &StagingBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create staging buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(engine.Device, StagingBuffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = GetMemoryType(engine, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	if (vkAllocateMemory(engine.Device, &allocInfo, nullptr, &StagingBufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate stagging buffer memory!");
	}

	return vkBindBufferMemory(engine.Device, StagingBuffer, StagingBufferMemory, 0);
}

VkResult VulkanBuffer::CreateBuffer(VulkanEngine& engine, VkDeviceSize BufferSize, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
{
	VkBufferCreateInfo buffer = {};
	buffer.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer.size = BufferSize;
	buffer.usage = usage;
	buffer.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	if (vkCreateBuffer(engine.Device, &buffer, nullptr, &Buffer) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(engine.Device, Buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = GetMemoryType(engine, memRequirements.memoryTypeBits, properties);
	if (vkAllocateMemory(engine.Device, &allocInfo, nullptr, &BufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate buffer memory!");
	}

	return vkBindBufferMemory(engine.Device, Buffer, BufferMemory, 0);
}

void VulkanBuffer::CopyBuffer(VulkanEngine& engine, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	VkCommandBuffer commandBuffer = engine.BeginSingleTimeCommand();

	VkBufferCopy copyRegion = {};
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	engine.EndSingleTimeCommand(commandBuffer);
}

void VulkanBuffer::MapMemory(VulkanEngine& engine, void* DataToCopy, VkDeviceSize BufferSize)
{
	void* data;
    vkMapMemory(engine.Device, StagingBufferMemory, 0, BufferSize, 0, &data);
    memcpy(data, DataToCopy, (size_t)BufferSize);
    vkUnmapMemory(engine.Device, StagingBufferMemory);
}

void VulkanBuffer::DestoryStagingBuffer(VulkanEngine& engine)
{
	vkDestroyBuffer(engine.Device, StagingBuffer, nullptr);
	vkFreeMemory(engine.Device, StagingBufferMemory, nullptr);

	StagingBuffer = VK_NULL_HANDLE;
	StagingBufferMemory = VK_NULL_HANDLE;
}

void VulkanBuffer::DestoryBuffer(VulkanEngine& engine)
{
	vkDestroyBuffer(engine.Device, Buffer, nullptr);
	vkFreeMemory(engine.Device, BufferMemory, nullptr);

	Buffer = VK_NULL_HANDLE;
	BufferMemory = VK_NULL_HANDLE;
}
