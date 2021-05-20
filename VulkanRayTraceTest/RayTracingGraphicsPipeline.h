#pragma once
#include "GraphicsPipeline.h"
class RayTracingGraphicsPipeline : public GraphicsPipeline
{
private:
protected:
	std::vector<VkRayTracingShaderGroupCreateInfoKHR> RayTraceShaders{};
public:
	VulkanBuffer raygenShaderBindingTable;
	VulkanBuffer missShaderBindingTable;
	VulkanBuffer hitShaderBindingTable;

	VkPhysicalDeviceRayTracingPipelinePropertiesKHR  rayTracingPipelineProperties{};
	VkPhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructureFeatures{};

	RayTracingGraphicsPipeline();
	RayTracingGraphicsPipeline(VulkanEngine& engine);
	~RayTracingGraphicsPipeline();

	virtual void UpdateGraphicsPipeLine(VulkanEngine& engine) override;
	void Destroy(VulkanEngine& engine) override;
};

