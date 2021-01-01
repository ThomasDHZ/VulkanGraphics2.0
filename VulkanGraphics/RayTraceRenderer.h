#pragma once
#include "VulkanEngine.h"
#include <vulkan/vulkan_core.h>

class RayTraceRenderer
{
private:
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

