#include "AccelerationStructure.h"

AccelerationStructure::AccelerationStructure()
{
}

AccelerationStructure::AccelerationStructure(VulkanEngine& engine)
{
	vkGetBufferDeviceAddressKHR = reinterpret_cast<PFN_vkGetBufferDeviceAddressKHR>(vkGetDeviceProcAddr(engine.Device, "vkGetBufferDeviceAddressKHR"));
	vkCmdBuildAccelerationStructuresKHR = reinterpret_cast<PFN_vkCmdBuildAccelerationStructuresKHR>(vkGetDeviceProcAddr(engine.Device, "vkCmdBuildAccelerationStructuresKHR"));
	vkBuildAccelerationStructuresKHR = reinterpret_cast<PFN_vkBuildAccelerationStructuresKHR>(vkGetDeviceProcAddr(engine.Device, "vkBuildAccelerationStructuresKHR"));
	vkCreateAccelerationStructureKHR = reinterpret_cast<PFN_vkCreateAccelerationStructureKHR>(vkGetDeviceProcAddr(engine.Device, "vkCreateAccelerationStructureKHR"));
	vkDestroyAccelerationStructureKHR = reinterpret_cast<PFN_vkDestroyAccelerationStructureKHR>(vkGetDeviceProcAddr(engine.Device, "vkDestroyAccelerationStructureKHR"));
	vkGetAccelerationStructureBuildSizesKHR = reinterpret_cast<PFN_vkGetAccelerationStructureBuildSizesKHR>(vkGetDeviceProcAddr(engine.Device, "vkGetAccelerationStructureBuildSizesKHR"));
	vkGetAccelerationStructureDeviceAddressKHR = reinterpret_cast<PFN_vkGetAccelerationStructureDeviceAddressKHR>(vkGetDeviceProcAddr(engine.Device, "vkGetAccelerationStructureDeviceAddressKHR"));
	vkCmdTraceRaysKHR = reinterpret_cast<PFN_vkCmdTraceRaysKHR>(vkGetDeviceProcAddr(engine.Device, "vkCmdTraceRaysKHR"));
	vkGetRayTracingShaderGroupHandlesKHR = reinterpret_cast<PFN_vkGetRayTracingShaderGroupHandlesKHR>(vkGetDeviceProcAddr(engine.Device, "vkGetRayTracingShaderGroupHandlesKHR"));
	vkCreateRayTracingPipelinesKHR = reinterpret_cast<PFN_vkCreateRayTracingPipelinesKHR>(vkGetDeviceProcAddr(engine.Device, "vkCreateRayTracingPipelinesKHR"));
}

AccelerationStructure::~AccelerationStructure()
{
}

void AccelerationStructure::AcclerationCommandBuffer(VulkanEngine& engine, VkAccelerationStructureBuildGeometryInfoKHR& AccelerationStructureBuildGeometryInfo, std::vector<VkAccelerationStructureBuildRangeInfoKHR>& AccelerationStructureBuildRangeInfo)
{
	VkCommandBufferAllocateInfo CommandBufferAllocateInfo{};
	CommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	CommandBufferAllocateInfo.commandPool = engine.CommandPool;
	CommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	CommandBufferAllocateInfo.commandBufferCount = 1;

	VkCommandBuffer cmdBuffer;
	vkAllocateCommandBuffers(engine.Device, &CommandBufferAllocateInfo, &cmdBuffer);

	VkCommandBufferBeginInfo cmdBufferBeginInfo{};
	cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	auto AccelerationStructureBuildRangeInfoPtr = AccelerationStructureBuildRangeInfo.data();
	vkBeginCommandBuffer(cmdBuffer, &cmdBufferBeginInfo);
	vkCmdBuildAccelerationStructuresKHR(cmdBuffer, 1, &AccelerationStructureBuildGeometryInfo, &AccelerationStructureBuildRangeInfoPtr);
	vkEndCommandBuffer(cmdBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cmdBuffer;
	VkFenceCreateInfo fenceCreateInfo{};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = 0;

	VkFence fence;
	vkCreateFence(engine.Device, &fenceCreateInfo, nullptr, &fence);
	vkQueueSubmit(engine.GraphicsQueue, 1, &submitInfo, fence);
	vkWaitForFences(engine.Device, 1, &fence, VK_TRUE, UINT64_MAX);
	vkDestroyFence(engine.Device, fence, nullptr);
	vkFreeCommandBuffers(engine.Device, engine.CommandPool, 1, &cmdBuffer);
}

void AccelerationStructure::CreateAccelerationStructure(VulkanEngine& engine, VkAccelerationStructureTypeKHR type, VkAccelerationStructureBuildSizesInfoKHR& buildSizeInfo)
{
	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = buildSizeInfo.accelerationStructureSize;
	bufferCreateInfo.usage = VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
	vkCreateBuffer(engine.Device, &bufferCreateInfo, nullptr, &AccelerationBuffer.Buffer);

	VkMemoryRequirements memoryRequirements{};
	vkGetBufferMemoryRequirements(engine.Device, AccelerationBuffer.Buffer, &memoryRequirements);

	VkMemoryAllocateFlagsInfo memoryAllocateFlagsInfo{};
	memoryAllocateFlagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
	memoryAllocateFlagsInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT_KHR;

	VkMemoryAllocateInfo memoryAllocateInfo{};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.pNext = &memoryAllocateFlagsInfo;
	memoryAllocateInfo.allocationSize = memoryRequirements.size;
	memoryAllocateInfo.memoryTypeIndex = engine.FindMemoryType(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	vkAllocateMemory(engine.Device, &memoryAllocateInfo, nullptr, &AccelerationBuffer.BufferMemory);
	vkBindBufferMemory(engine.Device, AccelerationBuffer.Buffer, AccelerationBuffer.BufferMemory, 0);

	VkAccelerationStructureCreateInfoKHR accelerationStructureCreate_info{};
	accelerationStructureCreate_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
	accelerationStructureCreate_info.buffer = AccelerationBuffer.Buffer;
	accelerationStructureCreate_info.size = buildSizeInfo.accelerationStructureSize;
	accelerationStructureCreate_info.type = type;
	vkCreateAccelerationStructureKHR(engine.Device, &accelerationStructureCreate_info, nullptr, &handle);

	VkAccelerationStructureDeviceAddressInfoKHR accelerationDeviceAddressInfo{};
	accelerationDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
	accelerationDeviceAddressInfo.accelerationStructure = handle;
	AccelerationBuffer.BufferDeviceAddress = vkGetAccelerationStructureDeviceAddressKHR(engine.Device, &accelerationDeviceAddressInfo);
}

void AccelerationStructure::Destroy(VulkanEngine& engine)
{
	vkFreeMemory(engine.Device, AccelerationBuffer.BufferMemory, nullptr);
	vkDestroyBuffer(engine.Device, AccelerationBuffer.Buffer, nullptr);
	vkDestroyAccelerationStructureKHR(engine.Device, handle, nullptr);

	AccelerationBuffer.BufferMemory = VK_NULL_HANDLE;
	AccelerationBuffer.Buffer = VK_NULL_HANDLE;
	handle = VK_NULL_HANDLE;
	AccelerationBuffer.BufferDeviceAddress = 0;
}