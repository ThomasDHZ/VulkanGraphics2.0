#pragma once
#include "Vulkanengine.h"
#include "VulkanBuffer.h"

struct AccelerationStructure
{
private:
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

public:
	VkAccelerationStructureKHR handle = VK_NULL_HANDLE;
	VulkanBuffer AccelerationBuffer;

	AccelerationStructure();
	AccelerationStructure(std::shared_ptr<VulkanEngine> engine);
	~AccelerationStructure();

	void AcclerationCommandBuffer(std::shared_ptr<VulkanEngine> engine, VkAccelerationStructureBuildGeometryInfoKHR& AccelerationStructureBuildGeometryInfo, std::vector<VkAccelerationStructureBuildRangeInfoKHR>& AccelerationStructureBuildRangeInfo);
	void CreateAccelerationStructure(std::shared_ptr<VulkanEngine> engine, VkAccelerationStructureTypeKHR type, VkAccelerationStructureBuildSizesInfoKHR& buildSizeInfo);
	void Destroy(std::shared_ptr<VulkanEngine> engine);
};

