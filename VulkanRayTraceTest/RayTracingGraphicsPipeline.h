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
	RayTracingGraphicsPipeline(std::shared_ptr<VulkanEngine> engine);
	~RayTracingGraphicsPipeline();

	virtual void UpdateGraphicsPipeLine(std::shared_ptr<VulkanEngine> engine) override;
	void Destroy(std::shared_ptr<VulkanEngine> engine) override;
};

