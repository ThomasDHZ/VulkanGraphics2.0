#pragma once
#include "VulkanEngine.h"
#include <vulkan/vulkan_core.h>
#include "VertexBuffer.h"
#include "IndicesBuffer.h"
#include "TransformBuffer.h"
#include "VulkanBuffer.h"
#include "VulkanBufferManager.h"
#include "RayTraceTexture.h"
#include "VulkanUniformBuffer.h"

struct RTUniformData {
	glm::mat4 viewInverse;
	glm::mat4 projInverse;
};

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
	VulkanBuffer TopLevelAccelerationBuffer;
	VkAccelerationStructureKHR BottomLevelAccelerationStructure;
	VkAccelerationStructureKHR TopLevelAccelerationStructure;
	uint64_t BottomLevelAccelerationDeviceAddress;
	uint64_t TopLevelAccelerationDeviceAddress;
	VkCommandBuffer RayTraceCommandBuffer;

	VkPhysicalDeviceProperties2 RTDeviceProperties;
	VkPhysicalDeviceFeatures2 RTPhysicalDeviceProperties;

	VkDescriptorSetLayout RayTraceDescriptorSetLayout;
	VkPipelineLayout RayTracePipelineLayout;
	VkPipeline RayTracePipeline;

	VulkanBuffer raygenShaderBindingTable;
	VulkanBuffer missShaderBindingTable;
	VulkanBuffer hitShaderBindingTable;

	VkDescriptorPool RTDescriptorPool;
	VkDescriptorSet RTDescriptorSet;

	RayTraceTexture RTTexture;

	VulkanUniformBuffer uniformBuffer;

	std::vector<VkRayTracingShaderGroupCreateInfoKHR> RayTraceShaders{};

	std::vector<char> ReadShaderFile(const std::string& filename);
	VkShaderModule CreateShaderModule(VulkanEngine& engine, const std::vector<char>& code);
	VkPipelineShaderStageCreateInfo LoadShader(VulkanEngine& engine, const std::string& filename, VkShaderStageFlagBits StageFlag);

	void InitializeBottomLevelAccelerationStructure(VulkanEngine& engine);
	void InitializeTopLevelAccelerationStructure(VulkanEngine& engine);
	void InitializeRayTracingPipeline(VulkanEngine& engine);
	void InitializeRayTracingShaderBindingTable(VulkanEngine& engine);
	void InitializeRayTracingDescriptorSet(VulkanEngine& engine);

public:
	RayTraceRenderer();
	RayTraceRenderer(VulkanEngine& engine);
	~RayTraceRenderer();

	void Draw();
};

