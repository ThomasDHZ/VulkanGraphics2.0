#include "VulkanBuffer.h"
#include <stdexcept>

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

VulkanBuffer::~VulkanBuffer()
{
}

VkResult VulkanBuffer::CreateBuffer(VulkanEngine& engine, VkDeviceSize bufferSize, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, void* BufferData)
{
	BufferSize = bufferSize;

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
	if (usage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT)
	{
		VkMemoryAllocateFlagsInfoKHR ExtendedAllocFlagsInfo{};
		ExtendedAllocFlagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO_KHR;
		ExtendedAllocFlagsInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT_KHR;
		allocInfo.pNext = &ExtendedAllocFlagsInfo;
	}
	if (vkAllocateMemory(engine.Device, &allocInfo, nullptr, &BufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate buffer memory!");
	}

	if (BufferData != nullptr)
	{
		CopyBufferToMemory(engine, BufferData, bufferSize);
	}

	return vkBindBufferMemory(engine.Device, Buffer, BufferMemory, 0);
}

void VulkanBuffer::CopyBuffer(VulkanEngine& engine, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	VkCommandBuffer commandBuffer = BeginSingleTimeCommand(engine);

	VkBufferCopy copyRegion = {};
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	EndSingleTimeCommand(engine, commandBuffer);
}

void VulkanBuffer::CopyBufferToMemory(VulkanEngine& engine, void* DataToCopy, VkDeviceSize BufferSize)
{
	void* data;
	vkMapMemory(engine.Device, BufferMemory, 0, BufferSize, 0, &data);
	memcpy(data, DataToCopy, (size_t)BufferSize);
	vkUnmapMemory(engine.Device, BufferMemory);
}

void VulkanBuffer::DestoryBuffer(VulkanEngine& engine)
{
	vkDestroyBuffer(engine.Device, Buffer, nullptr);
	vkFreeMemory(engine.Device, BufferMemory, nullptr);

	Buffer = VK_NULL_HANDLE;
	BufferMemory = VK_NULL_HANDLE;
}


VkCommandBuffer VulkanBuffer::BeginSingleTimeCommand(VulkanEngine& engine)
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = engine.RenderCommandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(engine.Device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

void VulkanBuffer::EndSingleTimeCommand(VulkanEngine& engine, VkCommandBuffer commandBuffer)
{
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(engine.GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(engine.GraphicsQueue);

	vkFreeCommandBuffers(engine.Device, engine.RenderCommandPool, 1, &commandBuffer);
}