#pragma once
#include "VulkanEngine.h"

struct ConstMeshInfo
{
	alignas(4) uint32_t MeshIndex;
	alignas(16) glm::mat4 proj = glm::mat4(1.0f);
	alignas(16) glm::mat4 view = glm::mat4(1.0f);
};

struct PrefilterConst
{
	alignas(16) glm::mat4 proj = glm::mat4(1.0f);
	alignas(16) glm::mat4 view = glm::mat4(1.0f);
	alignas(4) float MipLevel = 0.0f;
	alignas(4) uint32_t SampleCount = 1024;
	alignas(4) float FaceSize = 0.0f;
};

struct ConstSkyBoxView
{
	alignas(16) glm::mat4 proj = glm::mat4(1.0f);
	alignas(16) glm::mat4 view = glm::mat4(1.0f);
};

struct ConstBloomProperites
{
	alignas(4) uint32_t BloomPass = 0;
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

	virtual void UpdateGraphicsPipeLine(VulkanEngine& engine);
	virtual void Destroy(VulkanEngine& engine);
};