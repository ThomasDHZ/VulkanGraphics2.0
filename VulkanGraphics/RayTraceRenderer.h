#pragma once
#include "VulkanEngine.h"
#include <vulkan/vulkan_core.h>
#include "VertexBuffer.h"
#include "IndicesBuffer.h"
#include "TransformBuffer.h"
#include "VulkanBuffer.h"
#include "VulkanBufferManager.h"

class RayTraceRenderer
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

	VulkanBuffer BottomLevelAccelerationBuffer;
	VkAccelerationStructureKHR BottomLevelAccelerationStructure;
	uint64_t BottomLevelAccelerationDeviceAddress;
	VkCommandBuffer RayTraceCommandBuffer;

	void InitializeBottomLevelAccelerationStructure(VulkanEngine& engine);
	void InitializeTopLevelAccelerationStructure(VulkanEngine& engine);
	void InitializeRayTracingDescriptorSet(VulkanEngine& engine);
	void InitializeRayTracingPipeline(VulkanEngine& engine);
	void InitializeRayTracingShaderBindingTable(VulkanEngine& engine);

public:
	RayTraceRenderer();
	RayTraceRenderer(VulkanEngine& engine);
	~RayTraceRenderer();

	void Draw();
};

