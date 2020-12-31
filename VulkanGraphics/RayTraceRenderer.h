#pragma once
#include "VulkanEngine.h"

class RayTraceRenderer
{
private:
	void InitializeBottomLevelAccelerationStructure(VulkanEngine& Engine);
	void InitializeTopLevelAccelerationStructure(VulkanEngine& Engine);
	void InitializeRayTracingDescriptorSet(VulkanEngine& Engine);
	void InitializeRayTracingPipeline(VulkanEngine& Engine);
	void InitializeRayTracingShaderBindingTable(VulkanEngine& Engine);
public:
	RayTraceRenderer();
	RayTraceRenderer(VulkanEngine& Engine);
	~RayTraceRenderer();

	void Draw();
};

