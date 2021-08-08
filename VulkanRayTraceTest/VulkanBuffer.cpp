#include "VulkanBuffer.h"
#include <stdexcept>

uint32_t VulkanBuffer::GetMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(VulkanPtr::GetPhysicalDevice(), &memProperties);

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

VulkanBuffer::VulkanBuffer(VkDevice& device, VkPhysicalDevice& physicalDevice, VkDeviceSize BufferSize, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, void* BufferData)
{
	CreateBuffer(BufferSize, usage, properties, BufferData);
}

VulkanBuffer::~VulkanBuffer()
{
}

VkResult VulkanBuffer::CreateBuffer(VkDeviceSize bufferSize, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, void* BufferData)
{
	BufferSize = bufferSize;

	VkBufferCreateInfo buffer = {};
	buffer.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer.size = BufferSize;
	buffer.usage = usage;
	buffer.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	if (vkCreateBuffer(VulkanPtr::GetDevice(), &buffer, nullptr, &Buffer) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(VulkanPtr::GetDevice(), Buffer, &memRequirements);

	VkMemoryAllocateFlagsInfoKHR ExtendedAllocFlagsInfo{};
	ExtendedAllocFlagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO_KHR;
	ExtendedAllocFlagsInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT_KHR;

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = GetMemoryType(memRequirements.memoryTypeBits, properties);
	allocInfo.pNext = &ExtendedAllocFlagsInfo;

	if (vkAllocateMemory(VulkanPtr::GetDevice(), &allocInfo, nullptr, &BufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate buffer memory!");
	}

	if (BufferData != nullptr)
	{
		CopyBufferToMemory(BufferData, bufferSize);
	}

	return vkBindBufferMemory(VulkanPtr::GetDevice(), Buffer, BufferMemory, 0);
}

void VulkanBuffer::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	VkCommandBuffer commandBuffer = BeginSingleTimeCommand();

	VkBufferCopy copyRegion = {};
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	EndSingleTimeCommand(commandBuffer);
}

void VulkanBuffer::CopyBufferToMemory(void* DataToCopy, VkDeviceSize BufferSize)
{
	void* data;
	vkMapMemory(VulkanPtr::GetDevice(), BufferMemory, 0, BufferSize, 0, &data);
	memcpy(data, DataToCopy, (size_t)BufferSize);
	vkUnmapMemory(VulkanPtr::GetDevice(), BufferMemory);
}

void VulkanBuffer::DestoryBuffer(VkDevice& device)
{
	vkDestroyBuffer(device, Buffer, nullptr);
	vkFreeMemory(device, BufferMemory, nullptr);

	Buffer = VK_NULL_HANDLE;
	BufferMemory = VK_NULL_HANDLE;
}


VkCommandBuffer VulkanBuffer::BeginSingleTimeCommand()
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = VulkanPtr::GetCommandPool();
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(VulkanPtr::GetDevice(), &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

void VulkanBuffer::EndSingleTimeCommand(VkCommandBuffer commandBuffer)
{
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(VulkanPtr::GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(VulkanPtr::GetGraphicsQueue());

	vkFreeCommandBuffers(VulkanPtr::GetDevice(), VulkanPtr::GetCommandPool(), 1, &commandBuffer);
}