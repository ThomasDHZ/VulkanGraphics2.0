#pragma once
#include "VulkanEngine.h"

struct ConstMeshInfo
{
	alignas(4) uint32_t MeshIndex;
};

struct PrefilterConst
{
	alignas(4) float MipLevel = 0;
};

struct ConstSkyBoxView
{
	alignas(16) glm::mat4 view = glm::mat4(1.0f);
	alignas(16) glm::mat4 proj = glm::mat4(1.0f);
};

class GraphicsPipeline
{
protected:


public:
	VkDescriptorPool DescriptorPool = VK_NULL_HANDLE;
	VkDescriptorSetLayout DescriptorSetLayout = VK_NULL_HANDLE;
	VkDescriptorSet DescriptorSets = VK_NULL_HANDLE;
	VkPipelineLayout ShaderPipelineLayout = VK_NULL_HANDLE;
	VkPipeline ShaderPipeline = VK_NULL_HANDLE;

	GraphicsPipeline();
	~GraphicsPipeline();

	virtual void UpdateGraphicsPipeLine(VulkanEngine& renderer, const VkRenderPass& renderPass);
	virtual void Destroy(VulkanEngine& engine);
};